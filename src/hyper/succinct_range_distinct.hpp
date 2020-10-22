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

        template <typename INDEX_SIZE>
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
            std::vector<uint64_t> C;

            template <typename RLBWT_STR>
            void initialize(const RLBWT_STR &_rlbwt)
            {
                using CHAR_VEC = typename RLBWT_STR::char_vec_type;
                uint64_t CHARMAX = UINT8_MAX + 1;

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

                cs.resize(CHARMAX, 0);
                cs1.resize(CHARMAX, 0);
                cs2.resize(CHARMAX, 0);

                stool::FMIndex::constructC(bwt, C);
            }
            /*
            uint64_t getIntervals(INDEX_SIZE i, INDEX_SIZE j, std::vector<CharInterval<INDEX_SIZE>> &output)
            {
                using CHARINTV = CharInterval<INDEX_SIZE>;
                std::vector<CHARINTV> r;
                uint64_t k;
                uint64_t newJ = j + 1 == wt.size() ? wt.size() : j + 2;
                uint64_t p = 0;

                sdsl::interval_symbols(wt, i + 1, newJ, k, cs, cs1, cs2);

                bool b = j + 1 < wt.size();
                for (INDEX_SIZE x = 0; x < k; x++)
                {
                    INDEX_SIZE left = C[cs[x]] + cs1[x];
                    INDEX_SIZE right = left + (cs2[x] - cs1[x] - 1);

                    if (j + 1 == wt.size() && cs[x] == lastChar)
                    {
                        right++;
                        b = true;
                    }

                    output[p++] = CHARINTV(left, right, cs[x]);
                }
                if (!b)
                {
                    INDEX_SIZE num = wt.rank(wt.size(), lastChar) + 1;
                    INDEX_SIZE left = C[lastChar] + num - 1;
                    INDEX_SIZE right = left;
                    output[p++] = CHARINTV(left, right, lastChar);
                }

                return p;
            }
            */
            std::vector<CharInterval<INDEX_SIZE>> range_distinct(INDEX_SIZE i, INDEX_SIZE j)
            {
                std::vector<CharInterval<INDEX_SIZE>> r;
                uint64_t k;
                uint64_t newJ = j + 1 == wt.size() ? wt.size() : j + 2;

                //std::cout << "@[" << i << "/" << j << ", " << wt.size() << "]" << std::endl;

                sdsl::interval_symbols(wt, i + 1, newJ, k, cs, cs1, cs2);

                bool b = j + 1 < wt.size();
                for (uint64_t x = 0; x < k; x++)
                {
                    uint64_t fstRank = cs1[x] + 1;
                    uint64_t lastRank = cs2[x];

                    uint64_t left = wt.select(fstRank, cs[x]) - 1;
                    //std::cout << "@[" << fstRank << ", " << left << ", " << cs[x] << "]" << std::endl;

                    uint64_t right = wt.select(lastRank, cs[x]) - 1;
                    //std::cout << "@[" << lastRank << ", " << right << ", " << cs[x] << "]" << std::endl;

                    //std::cout << "XX" << std::endl;

                    //uint64_t right = C[cs[x]] + cs2[x]+1;

                    if (j + 1 == wt.size() && cs[x] == lastChar)
                    {
                        //std::cout << "++" << std::endl;
                        right = wt.size() - 1;
                        b = true;
                    }

                    r.push_back(CharInterval<INDEX_SIZE>(left, right, cs[x]));
                }
                if (!b)
                {
                    //uint64_t num = wt.rank(wt.size(), lastChar) + 1;
                    uint64_t left = wt.size() - 1;
                    uint64_t right = left;

                    r.push_back(CharInterval<INDEX_SIZE>(left, right, lastChar));
                }
                //std::cout << "END" << std::endl;

                return r;
            }
        };
    } // namespace lcp_on_rlbwt
} // namespace stool