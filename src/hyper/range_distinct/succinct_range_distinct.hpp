#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>

#include "stool/src/debug.hpp"
#include "stool/src/elias_fano_vector.hpp"
#include "../../beller/char_interval.hpp"
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
            const sdsl::wt_huff<>* wt;
            uint8_t lastChar;
            uint64_t size;
            //sdsl::int_vector<> bwt;
            std::vector<uint8_t> cs;
            std::vector<uint64_t> cs1;
            std::vector<uint64_t> cs2;
            std::vector<uint64_t> C;

            void initialize(const sdsl::wt_huff<>* _wt, const sdsl::int_vector<> *_bwt)
            {
                wt = _wt;
                size = wt->size();
                lastChar = (*_bwt)[_bwt->size()-1];
                //using CHAR_VEC = typename RLBWT_STR::char_vec_type;
                uint64_t CHARMAX = UINT8_MAX + 1;


                cs.resize(CHARMAX, 0);
                cs1.resize(CHARMAX, 0);
                cs2.resize(CHARMAX, 0);

                stool::FMIndex::constructC(*_bwt, C);
            }
            uint64_t range_distinct(INDEX_SIZE i, INDEX_SIZE j, std::vector<CharInterval<INDEX_SIZE>> &output)
            {
                //std::vector<CharInterval<INDEX_SIZE>> r;
                uint64_t k;
                uint64_t newJ = j + 1 == size ? size : j + 2;


                sdsl::interval_symbols(*wt, i + 1, newJ, k, cs, cs1, cs2);

                bool b = j + 1 < size;
                for (uint64_t x = 0; x < k; x++)
                {
                    uint64_t fstRank = cs1[x] + 1;
                    uint64_t lastRank = cs2[x];

                    uint64_t left = wt->select(fstRank, cs[x]) - 1;

                    uint64_t right = wt->select(lastRank, cs[x]) - 1;

                    if (j + 1 == size && cs[x] == lastChar)
                    {
                        //std::cout << "++" << std::endl;
                        right = size - 1;
                        b = true;
                    }

                    output[x] = CharInterval<INDEX_SIZE>(left, right, cs[x]);
                }
                if (!b)
                {
                    uint64_t left = size - 1;
                    uint64_t right = left;

                    output[k++] = CharInterval<INDEX_SIZE>(left, right, lastChar);
                }

                return k;
            }
        };
    } // namespace lcp_on_rlbwt
} // namespace stool