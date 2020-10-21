#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>

#include "stool/src/debug.hpp"
#include "stool/src/elias_fano_vector.hpp"
#include "../beller/char_interval.hpp"
#include <sdsl/rmq_support.hpp> // include header for range minimum queries
namespace stool
{
    namespace lcp_on_rlbwt
    {
        class SuccinctRangeDistinctDataStructure
        {
        public:
            //std::vector<uint64_t> C;
            sdsl::wt_huff<> wt;
            uint8_t lastChar;
            sdsl::int_vector<> bwt;
            std::vector<uint8_t> cs;
            std::vector<uint64_t> cs1;
            std::vector<uint64_t> cs2;

            template <typename RLBWT_STR, typename INDEX_SIZE = uint64_t>
            void initialize(const RLBWT_STR &_rlbwt)
            {
                using CHAR_VEC = typename RLBWT_STR::char_vec_type;
                bwt.width(8);

                uint64_t size = _rlbwt.rle_size();
                bwt.resize(size);
                const CHAR_VEC *_char_vec = _rlbwt.get_char_vec();
                for (uint64_t i = 0; i < size; i++)
                {
                    bwt[i] = (*_char_vec)[i];
                }
                this->lastChar = bwt[size - 1];
                //stool::FMIndex::constructC(bwt, C);
                construct_im(wt, bwt);

                                cs.resize(256, 0);
                cs1.resize(256, 0);
                cs2.resize(256, 0);

            }

            std::vector<CharInterval> range_distinct(uint64_t i, uint64_t j)
            {
                std::vector<CharInterval> r;
                uint64_t k;
                uint64_t newJ = j + 1 == wt.size() ? wt.size() : j + 2;


                //std::cout << "@[" << i << "/" << j << ", " << wt.size() << "]" << std::endl;

                sdsl::interval_symbols(wt, i + 1, newJ, k, cs, cs1, cs2);

                bool b = j + 1 < wt.size();
                for (uint64_t x = 0; x < k; x++)
                {
                    uint64_t fstRank = cs1[x]+1;
                    uint64_t lastRank = cs2[x];

                    uint64_t left = wt.select(fstRank, cs[x])-1;
                //std::cout << "@[" << fstRank << ", " << left << ", " << cs[x] << "]" << std::endl;

                    uint64_t right = wt.select(lastRank, cs[x])-1;
                //std::cout << "@[" << lastRank << ", " << right << ", " << cs[x] << "]" << std::endl;

                //std::cout << "XX" << std::endl;

                    //uint64_t right = C[cs[x]] + cs2[x]+1;

                    if (j + 1 == wt.size() && cs[x] == lastChar)
                    {
                        //std::cout << "++" << std::endl;
                        right = wt.size()-1;
                        b = true;
                    }

                    r.push_back(CharInterval(left, right, cs[x]));
                }
                if (!b)
                {
                    //uint64_t num = wt.rank(wt.size(), lastChar) + 1;
                    uint64_t left = wt.size() - 1;
                    uint64_t right = left;

                    r.push_back(CharInterval(left, right, lastChar));
                }
                //std::cout << "END" << std::endl;

                return r;
            }
        };
    } // namespace lcp_on_rlbwt
} // namespace stool