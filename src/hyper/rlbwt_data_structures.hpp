#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <vector>
#include <type_traits>
#include "./hyper_set.hpp"
#include "range_distinct/range_distinct_on_rlbwt.hpp"

namespace stool
{
    namespace lcp_on_rlbwt
    {

        template <typename RLBWT_STR, typename INDEX_SIZE>
        class RLBWTDataStructures
        {
        public:
            using CHAR = typename RLBWT_STR::char_type;
            using CHARVEC = typename RLBWT_STR::char_vec_type;
            using RINTERVAL = RInterval<INDEX_SIZE>;

            using UCHAR = typename std::make_unsigned<CHAR>::type;

            const RLBWT_STR &_rlbwt;
            RangeDistinctDataStructureOnRLBWT<RLBWT_STR, INDEX_SIZE> rangeOnRLBWT;
            sdsl::wt_huff<> wt;
            sdsl::int_vector<> bwt;
            std::vector<uint64_t> C;
            stool::EliasFanoVector fposSortedArray;
            std::vector<INDEX_SIZE> fposArray;
            bool lightWeight = false;

            RLBWTDataStructures(const RLBWT_STR &__rlbwt, bool _lightWeight) : _rlbwt(__rlbwt)
            {
                lightWeight = _lightWeight;
                //uint64_t CHARMAX = UINT8_MAX + 1;
                uint64_t runSize = _rlbwt.rle_size();

                if (lightWeight)
                {
                    RLBWTDataStructures::construct_C(__rlbwt, this->C);
                    RLBWTDataStructures::construct_sorted_fpos_array(__rlbwt, this->fposSortedArray);
                }
                else
                {
                    std::vector<INDEX_SIZE> v1 = RLBWTFunctions::construct_fpos_array<RLBWT_STR, INDEX_SIZE>(_rlbwt);
                    this->fposArray.swap(v1);
                }

                //uint64_t runSize = _rlbwt.rle_size();

                bwt.width(8);
                bwt.resize(runSize);
                const CHARVEC *chars = __rlbwt.get_char_vec();
                for (uint64_t i = 0; i < runSize; i++)
                {
                    bwt[i] = (*chars)[i];
                }
                construct_im(wt, bwt);
                rangeOnRLBWT.initialize(&_rlbwt, &wt, &bwt);
            }
            static void construct_C(const RLBWT_STR &rlbwt, std::vector<uint64_t> &C)
            {
                uint64_t CHARMAX = UINT8_MAX + 1;
                std::vector<uint64_t> CK;
                CK.resize(CHARMAX, 0);
                C.resize(CHARMAX, 0);
                uint64_t rle = rlbwt.rle_size();
                for (uint64_t i = 0; i < rle; i++)
                {
                    uint8_t c = rlbwt.get_char_by_run_index(i);
                    //uint64_t l = rlbwt.get_run(i);
                    CK[c] += 1;
                }
                for (uint64_t i = 1; i < CHARMAX; i++)
                {
                    C[i] = C[i - 1] + CK[i - 1];
                }
            }

            static void construct_sorted_fpos_array(const RLBWT_STR &rlbwt, stool::EliasFanoVector &output)
            {
                uint64_t CHARMAX = UINT8_MAX + 1;
                std::vector<uint64_t> X;
                std::vector<uint64_t> CK;
                std::vector<uint64_t> C;
                std::vector<uint64_t> numVec;
                std::vector<uint64_t> numC;

                std::vector<uint64_t> CK3;

                CK.resize(CHARMAX, 0);
                numVec.resize(CHARMAX, 0);
                numC.resize(CHARMAX, 0);
                C.resize(CHARMAX, 0);
                CK3.resize(CHARMAX, 0);

                uint64_t rle = rlbwt.rle_size();
                for (uint64_t i = 0; i < rle; i++)
                {
                    uint8_t c = rlbwt.get_char_by_run_index(i);
                    uint64_t l = rlbwt.get_run(i);
                    CK[c] += l;
                    numVec[c]++;
                }
                for (uint64_t i = 1; i < CHARMAX; i++)
                {
                    C[i] = C[i - 1] + CK[i - 1];
                    numC[i] = numC[i - 1] + numVec[i - 1];
                }
                X.resize(rle);
                for (uint64_t i = 0; i < rle; i++)
                {
                    uint8_t c = rlbwt.get_char_by_run_index(i);
                    uint64_t l = rlbwt.get_run(i);
                    X[numC[c]] = C[c];
                    numC[c]++;
                    C[c] += l;
                }
                output.construct(&X);
            }
            INDEX_SIZE get_fpos(INDEX_SIZE index, INDEX_SIZE diff)
            {
                if (this->lightWeight)
                {
                    INDEX_SIZE rank1 = wt.rank(index + 1, this->bwt[index]);
                    uint64_t xx = C[this->bwt[index]] + rank1;
                    INDEX_SIZE begin_pos2 = this->fposSortedArray[xx] + diff;
                    return begin_pos2;
                }
                else
                {
                    uint64_t z = this->fposArray[index] + diff;
                    return z;
                }
            }
            RINTERVAL getIntervalOnL(const RINTERVAL &interval)
            {
                RINTERVAL output;
                INDEX_SIZE begin_pos = this->get_fpos(interval.beginIndex, interval.beginDiff);
                output.beginIndex = _rlbwt.get_lindex_containing_the_position(begin_pos);
                output.beginDiff = begin_pos - _rlbwt.get_lpos(output.beginIndex);

                INDEX_SIZE end_pos = this->get_fpos(interval.endIndex, interval.endDiff);

                output.endIndex = _rlbwt.get_lindex_containing_the_position(end_pos);
                output.endDiff = end_pos - _rlbwt.get_lpos(output.endIndex);
                return output;
            }
        };
    } // namespace lcp_on_rlbwt
} // namespace stool