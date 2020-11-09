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
            IntervalTemporary<INDEX_SIZE> intervalTemporary;

            RLBWTDataStructures(const RLBWT_STR &__rlbwt, bool _lightWeight) : _rlbwt(__rlbwt)
            {
                lightWeight = _lightWeight;
                //uint64_t CHARMAX = UINT8_MAX + 1;
                uint64_t runSize = _rlbwt.rle_size();
                intervalTemporary.initialize();

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
            void computeNextIntervals(const RINTERVAL &w, bool isWeiner, WeinerDataStructures<INDEX_SIZE, CHAR> &wds, IntervalTemporary<INDEX_SIZE> &output)
            {
                RINTERVAL frontL = this->getIntervalOnL(w);
                uint64_t resultCount = this->rangeOnRLBWT.range_distinct(frontL, wds.weinerTmpVec, wds.charTmpVec);

                for (uint64_t i = 0; i < resultCount; i++)
                {
                    UCHAR c = wds.charTmpVec[i];
                    auto &it = wds.weinerTmpVec[i];
                    bool skip = false;
                    if (isWeiner)
                    {
                        skip = !wds.checkWeinerInterval(it, this->_rlbwt);
                    }
                    else
                    {
                        skip = !output.occur(c);
                    }
                    if (!skip)
                    {
                        output.push(it, c);
                    }
                }
            }
            void computeFirstLCPIntervalSet(HyperSet<INDEX_SIZE> &output, WeinerDataStructures<INDEX_SIZE, CHAR> &wds)
            {
                output.clear();
                RINTERVAL lcpIntv;
                size_t minIndex = this->_rlbwt.get_end_rle_lposition();

                uint64_t maxIndex = 0;
                UCHAR c = 0;
                for (uint64_t i = 0; i < this->_rlbwt.rle_size(); i++)
                {
                    UCHAR c2 = this->_rlbwt.get_char_by_run_index(i);
                    if (c < c2)
                    {
                        c = c2;
                        maxIndex = i;
                    }
                    else if (c == c2)
                    {
                        maxIndex = i;
                    }
                }
                uint64_t diff = this->_rlbwt.get_run(maxIndex) - 1;
                lcpIntv.beginIndex = minIndex;
                lcpIntv.beginDiff = 0;
                lcpIntv.endIndex = maxIndex;
                lcpIntv.endDiff = diff;

                uint64_t begin_lindex = 0;
                uint64_t begin_diff = 0;
                uint64_t end_lindex = this->_rlbwt.rle_size() - 1;
                uint64_t end_diff = this->_rlbwt.get_run(end_lindex) - 1;

                //vector<RINTERVAL> results = range_distinct(frontL, charOutputVec);
                RINTERVAL tmpArg;
                tmpArg.beginIndex = begin_lindex;
                tmpArg.beginDiff = begin_diff;
                tmpArg.endIndex = end_lindex;
                tmpArg.endDiff = end_diff;
                //std::vector<CHAR> charOutputVec;
                uint64_t resultCount = this->rangeOnRLBWT.range_distinct(tmpArg, wds.weinerTmpVec, wds.charTmpVec);
                assert(resultCount > 0);

                //auto weinerIntervals = RangeDistinctDataStructureOnRLBWT<RLBWT_STR, INDEX_SIZE, SuccinctRangeDistinctDataStructure<INDEX_SIZE>>::range_distinct(_rlbwt, srdds, begin_lindex, begin_diff, end_lindex, end_diff, charOutputVec);
                uint64_t counter = 0;
                for (uint64_t x = 0; x < resultCount; x++)
                {
                    auto &it = wds.weinerTmpVec[x];
                    if (wds.checkWeinerInterval(it, this->_rlbwt))
                    {
                        output.push_weiner(it);

                        counter++;
                    }
                }

                output.push(lcpIntv, counter);
            }
            void computeNextLCPIntervalSet(const RINTERVAL &lcpIntv, const std::vector<RINTERVAL> &weinerVec, uint64_t weinerVecSize, uint64_t rank, HyperSet<INDEX_SIZE> &outputSet, WeinerDataStructures<INDEX_SIZE, CHAR> &wds)
            {
                intervalTemporary.clearWeinerTmpVec();
                uint64_t i = rank;

                INDEX_SIZE lcpIntvBeginPos = this->get_fpos(lcpIntv.beginIndex, lcpIntv.beginDiff);
                INDEX_SIZE lcpIntvEndPos = this->get_fpos(lcpIntv.endIndex, lcpIntv.endDiff);

                while (i < weinerVecSize)
                {
                    INDEX_SIZE weinerBeginPos = this->get_fpos(weinerVec[i].beginIndex, weinerVec[i].beginDiff);
                    INDEX_SIZE weinerEndPos = this->get_fpos(weinerVec[i].endIndex, weinerVec[i].endDiff);
                    if (lcpIntvBeginPos <= weinerBeginPos && weinerEndPos <= lcpIntvEndPos)
                    {
                        this->computeNextIntervals(weinerVec[i], true, wds, this->intervalTemporary);
                    }
                    else
                    {
                        break;
                    }

                    i++;
                }
                this->computeNextIntervals(lcpIntv, false, wds, this->intervalTemporary);

                intervalTemporary.move(outputSet);
            }
            void computeNextLCPIntervalSet(HyperSet<INDEX_SIZE> &inputSet, HyperSet<INDEX_SIZE> &output, WeinerDataStructures<INDEX_SIZE, CHAR> &wds)
            {
                output.clear();
                uint64_t rank = 0;
                //const std::vector<RINTERVAL> * lcpIntvVec = inputSet.getLcpIntvVec();
                for (uint64_t i = 0; i < inputSet.lcpIntvCount; i++)
                {
                    this->computeNextLCPIntervalSet(inputSet.lcpIntvVec[i], inputSet.weinerVec, inputSet.weinerCount, rank, output, wds);
                    rank += inputSet.widthVec[i];
                }
            }
        };
    } // namespace lcp_on_rlbwt
} // namespace stool