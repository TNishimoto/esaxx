#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <vector>
#include <type_traits>
#include "hyper_set.hpp"

namespace stool
{
    namespace lcp_on_rlbwt
    {

        template <typename RLBWT_STR, typename INDEX_SIZE>
        class HyperSetConstructor
        {
        public:
            using CHAR = typename RLBWT_STR::char_type;
            using CHARVEC = typename RLBWT_STR::char_vec_type;

            using UCHAR = typename std::make_unsigned<CHAR>::type;
            using WEINER = WeinerInterval<INDEX_SIZE>;

            const RLBWT_STR &_rlbwt;
            std::vector<bool> checkerArray;
            //SuccinctRangeDistinctDataStructure<INDEX_SIZE> srdds;
            RangeDistinctDataStructureOnRLBWT<RLBWT_STR, INDEX_SIZE> rangeOnRLBWT;
            IntervalTemporary<INDEX_SIZE> intervalTemporary;
            bool lightWeight = false;

            std::vector<CHAR> charTmpVec;
            vector<WEINER> weinerTmpVec;

            sdsl::wt_huff<> wt;
            sdsl::int_vector<> bwt;

            std::vector<uint64_t> C;
            stool::EliasFanoVector fposSortedArray;
            std::vector<INDEX_SIZE> fposArray;

            uint64_t current_lcp = 0;
            uint64_t strSize = 0;
            uint64_t total_counter = 0;
            HyperSet<INDEX_SIZE> hyperSet;
            HyperSet<INDEX_SIZE> hyperTmpSet;
            uint64_t debugCounter = 0;

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

            HyperSetConstructor(const RLBWT_STR &__rlbwt, bool _lightWeight) : _rlbwt(__rlbwt)
            {
                lightWeight = _lightWeight;
                uint64_t CHARMAX = UINT8_MAX + 1;
                intervalTemporary.initialize();

                if (lightWeight)
                {
                    HyperSetConstructor::construct_C(__rlbwt, this->C);
                    HyperSetConstructor::construct_sorted_fpos_array(__rlbwt, this->fposSortedArray);
                }
                else
                {
                    std::vector<INDEX_SIZE> v1 = RLBWTFunctions::construct_fpos_array<RLBWT_STR, INDEX_SIZE>(_rlbwt);
                    this->fposArray.swap(v1);
                }

                uint64_t runSize = _rlbwt.rle_size();
                this->checkerArray.resize(runSize, false);
                //srdds.initialize(__rlbwt);

                this->strSize = this->_rlbwt.str_size();

                //charIntervalTmpVec.resize(CHARMAX);
                charTmpVec.resize(CHARMAX);
                weinerTmpVec.resize(CHARMAX);

                bwt.width(8);
                bwt.resize(runSize);
                const CHARVEC *chars = __rlbwt.get_char_vec();
                for (uint64_t i = 0; i < runSize; i++)
                {
                    bwt[i] = (*chars)[i];
                }
                construct_im(wt, bwt);
                rangeOnRLBWT.initialize(&_rlbwt, &wt);
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

                //assert(z == begin_pos2);
            }
            WEINER getIntervalOnL(WEINER &interval)
            {
                WEINER output;
                INDEX_SIZE begin_pos = this->get_fpos(interval.beginIndex, interval.beginDiff);
                output.beginIndex = _rlbwt.get_lindex_containing_the_position(begin_pos);
                output.beginDiff = begin_pos - _rlbwt.get_lpos(output.beginIndex);

                INDEX_SIZE end_pos = this->get_fpos(interval.endIndex, interval.endDiff);

                output.endIndex = _rlbwt.get_lindex_containing_the_position(end_pos);
                output.endDiff = end_pos - _rlbwt.get_lpos(output.endIndex);
                return output;
            }
            bool checkWeinerInterval(WEINER &w)
            {
                bool b = _rlbwt.get_run(w.endIndex) == (w.endDiff + 1);
                if (!b || !this->checkerArray[w.endIndex])
                {
                    if (b)
                    {
                        this->checkerArray[w.endIndex] = true;
                    }
                    return true;
                }
                else
                {
                    return false;
                }
            }
            void computeNextIntervals(WEINER &w, bool isWeiner)
            {
                debugCounter++;
                WEINER frontL = this->getIntervalOnL(w);
                uint64_t resultCount = rangeOnRLBWT.range_distinct(frontL, weinerTmpVec, charTmpVec);

                //vector<WEINER> results = range_distinct(frontL);
                for (uint64_t i = 0; i < resultCount; i++)
                {
                    UCHAR c = charTmpVec[i];
                    auto &it = weinerTmpVec[i];
                    bool skip = false;
                    if (isWeiner)
                    {
                        skip = !checkWeinerInterval(it);
                    }
                    if (!skip)
                    {
                        intervalTemporary.push(it, c);
                    }

                    //this->weinerTmpVec[c].push_back(intervals[i]);
                }
            }

            void computeNextSet(WEINER &lcpIntv, std::vector<WEINER> &weinerVec, uint64_t weinerVecSize, uint64_t rank, HyperSet<INDEX_SIZE> &outputSet)
            {
                intervalTemporary.clearWeinerTmpVec();
                computeNextIntervals(lcpIntv, false);

                uint64_t i = rank;

                INDEX_SIZE begin_pos1 = this->get_fpos(lcpIntv.beginIndex, lcpIntv.beginDiff);
                INDEX_SIZE end_pos1 = this->get_fpos(lcpIntv.endIndex, lcpIntv.endDiff);

                while (i < weinerVecSize)
                {
                    INDEX_SIZE begin_pos2 = this->get_fpos(weinerVec[i].beginIndex, weinerVec[i].beginDiff);
                    INDEX_SIZE end_pos2 = this->get_fpos(weinerVec[i].endIndex, weinerVec[i].endDiff);
                    if (begin_pos1 <= begin_pos2 && end_pos2 <= end_pos1)
                    {
                        computeNextIntervals(weinerVec[i], true);
                    }
                    else
                    {
                        break;
                    }

                    i++;
                }

                intervalTemporary.move(outputSet);
            }
            void computeFirstSet(HyperSet<INDEX_SIZE> &output)
            {
                output.clear();
                WEINER lcpIntv;
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
                /*
                auto iter2 = std::max_element(this->fposArray.begin(), this->fposArray.end());
                size_t maxIndexX = std::distance(this->fposArray.begin(), iter2);
                std::cout << maxIndex << "/" << maxIndexX << std::endl;
                assert(maxIndex == maxIndexX);
                */
                uint64_t diff = _rlbwt.get_run(maxIndex) - 1;
                lcpIntv.beginIndex = minIndex;
                lcpIntv.beginDiff = 0;
                lcpIntv.endIndex = maxIndex;
                lcpIntv.endDiff = diff;

                uint64_t begin_lindex = 0;
                uint64_t begin_diff = 0;
                uint64_t end_lindex = _rlbwt.rle_size() - 1;
                uint64_t end_diff = _rlbwt.get_run(end_lindex) - 1;

                //vector<WEINER> results = range_distinct(frontL, charOutputVec);
                WEINER tmpArg;
                tmpArg.beginIndex = begin_lindex;
                tmpArg.beginDiff = begin_diff;
                tmpArg.endIndex = end_lindex;
                tmpArg.endDiff = end_diff;
                //std::vector<CHAR> charOutputVec;
                uint64_t resultCount = rangeOnRLBWT.range_distinct(tmpArg, weinerTmpVec, charTmpVec);
                assert(resultCount > 0);

                //auto weinerIntervals = RangeDistinctDataStructureOnRLBWT<RLBWT_STR, INDEX_SIZE, SuccinctRangeDistinctDataStructure<INDEX_SIZE>>::range_distinct(_rlbwt, srdds, begin_lindex, begin_diff, end_lindex, end_diff, charOutputVec);
                uint64_t counter = 0;
                for (uint64_t x = 0; x < resultCount; x++)
                {
                    auto &it = weinerTmpVec[x];
                    if (checkWeinerInterval(it))
                    {
                        output.push_weiner(it);

                        counter++;
                    }
                }

                output.push(lcpIntv, counter);
            }

            void computeNextSet(HyperSet<INDEX_SIZE> &inputSet, HyperSet<INDEX_SIZE> &output)
            {
                output.clear();
                uint64_t rank = 0;
                for (uint64_t i = 0; i < inputSet.lcpIntvCount; i++)
                {
                    computeNextSet(inputSet.lcpIntvVec[i], inputSet.weinerVec, inputSet.weinerCount, rank, output);
                    rank += inputSet.widthVec[i];
                }
            }
            void process()
            {
                if (current_lcp == 0)
                {
                    this->computeFirstSet(hyperTmpSet);
                    hyperSet.swap(hyperTmpSet);
                }
                else
                {
                    this->computeNextSet(hyperSet, hyperTmpSet);
                    hyperSet.swap(hyperTmpSet);
                }

                total_counter += hyperSet.weinerCount;
                current_lcp++;
                assert(hyperSet.weinerCount > 0);
            }
            bool isStop()
            {
                return strSize == total_counter;
            }
            bool checkMaximalRepeat(WEINER &lcpIntv)
            {
                WEINER it = this->getIntervalOnL(lcpIntv);
                uint8_t fstChar = this->_rlbwt.get_char_by_run_index(it.beginIndex);
                uint8_t lstChar = this->_rlbwt.get_char_by_run_index(it.endIndex);
                if (fstChar == lstChar)
                {
                    /*
                    uint64_t p1 = srdds.wt.rank(it.beginIndex + 1, fstChar);
                    uint64_t p2 = srdds.wt.rank(it.endIndex + 1, fstChar);

                    uint64_t p1 = srdds.wt.rank(it.beginIndex + 1, fstChar);
                    uint64_t p2 = srdds.wt.rank(it.endIndex + 1, fstChar);
                    */

                    if (it.beginIndex != it.endIndex)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return true;
                }
            }

            static std::vector<uint64_t> constructLCPArray(const RLBWT_STR &__rlbwt, bool lightWeight)
            {
                HyperSetConstructor<RLBWT_STR, INDEX_SIZE> hsc(__rlbwt, lightWeight);
                std::vector<uint64_t> r;
                r.resize(hsc.strSize, 0);

                while (!hsc.isStop())
                {
                    hsc.process();
                    for (uint64_t i = 0; i < hsc.hyperSet.weinerCount; i++)
                    {
                        auto &it = hsc.hyperSet.weinerVec[i];
                        uint64_t pos = hsc.get_fpos(it.endIndex, it.endDiff) + 1;
                        if (pos < r.size())
                        {
                            assert(r[pos] == 0);
                            r[pos] = hsc.current_lcp - 1;
                        }
                    }
                }
                return r;
            }
            static std::vector<stool::LCPInterval<uint64_t>> constructLCPIntervals(const RLBWT_STR &__rlbwt, bool lightWeight)
            {
                HyperSetConstructor<RLBWT_STR, INDEX_SIZE> hsc(__rlbwt, lightWeight);
                std::vector<stool::LCPInterval<uint64_t>> r;

                while (!hsc.isStop())
                {
                    hsc.process();
                    for (uint64_t i = 0; i < hsc.hyperSet.lcpIntvCount; i++)
                    {
                        auto &it = hsc.hyperSet.lcpIntvVec[i];
                        uint64_t beg = hsc.get_fpos(it.beginIndex, it.beginDiff);
                        uint64_t end = hsc.get_fpos(it.endIndex, it.endDiff);
                        r.push_back(stool::LCPInterval<uint64_t>(beg, end, hsc.current_lcp - 1));
                    }
                }
                return r;

                //return weiner.enumerateLCPInterval();
            }

            static uint64_t outputMaximalSubstrings(const RLBWT_STR &__rlbwt, std::ofstream &out, bool lightWeight)
            {
                HyperSetConstructor<RLBWT_STR, INDEX_SIZE> hsc(__rlbwt, lightWeight);
                uint64_t count = 0;

                while (!hsc.isStop())
                {
                    hsc.process();
                    /*
                    if (hsc.current_lcp % 100 == 0)
                    {
                        std::cout << "LCP = " << (hsc.current_lcp - 1) << ", LCP Interval count = " << hsc.hyperSet.lcpIntvCount << std::endl;
                    }
                    */
                    for (uint64_t i = 0; i < hsc.hyperSet.lcpIntvCount; i++)
                    {
                        auto &it = hsc.hyperSet.lcpIntvVec[i];
                        if (hsc.checkMaximalRepeat(it))
                        {
                            uint64_t beg = hsc.get_fpos(it.beginIndex, it.beginDiff);
                            uint64_t end = hsc.get_fpos(it.endIndex, it.endDiff);
                            stool::LCPInterval<uint64_t> newLCPIntv(beg, end, hsc.current_lcp - 1);
                            count++;
                            out.write(reinterpret_cast<const char *>(&newLCPIntv), sizeof(stool::LCPInterval<INDEX_SIZE>));
                        }
                    }
                }
                //double px = hsc.rangeOnRLBWT.total_cover / hsc.rangeOnRLBWT.num;

                std::cout << "@" << hsc.rangeOnRLBWT.total_cover1 << "/" << hsc.rangeOnRLBWT.num1 << std::endl;
                std::cout << "@" << hsc.rangeOnRLBWT.total_cover2 << "/" << hsc.rangeOnRLBWT.num2 << std::endl;

                std::cout << "Range Distinct Count = " << hsc.debugCounter << "/" << __rlbwt.str_size()  << std::endl;

                uint64_t dx = __rlbwt.get_end_rle_lposition();
                uint64_t dollerPos = __rlbwt.get_lpos(dx);
                auto last = stool::LCPInterval<INDEX_SIZE>(dollerPos, dollerPos, __rlbwt.str_size());
                out.write(reinterpret_cast<const char *>(&last), sizeof(stool::LCPInterval<INDEX_SIZE>));
                count += 1;
                return count;
            }
            static std::vector<stool::LCPInterval<uint64_t>> computeMaximalSubstrings(const RLBWT_STR &__rlbwt, bool lightWeight)
            {
                HyperSetConstructor<RLBWT_STR, INDEX_SIZE> hsc(__rlbwt, lightWeight);
                std::vector<stool::LCPInterval<uint64_t>> r;

                while (!hsc.isStop())
                {
                    hsc.process();
                    if (hsc.current_lcp % 100 == 0)
                    {
                        std::cout << "LCP = " << (hsc.current_lcp - 1) << ", LCP Interval count = " << hsc.hyperSet.lcpIntvCount << std::endl;
                    }
                    for (uint64_t i = 0; i < hsc.hyperSet.lcpIntvCount; i++)
                    {
                        auto &it = hsc.hyperSet.lcpIntvVec[i];
                        if (hsc.checkMaximalRepeat(it))
                        {
                            uint64_t beg = hsc.get_fpos(it.beginIndex, it.beginDiff);
                            uint64_t end = hsc.get_fpos(it.endIndex, it.endDiff);
                            stool::LCPInterval<uint64_t> newLCPIntv(beg, end, hsc.current_lcp - 1);
                            r.push_back(newLCPIntv);
                        }
                    }
                }
                uint64_t dx = __rlbwt.get_end_rle_lposition();
                uint64_t dollerPos = __rlbwt.get_lpos(dx);
                auto last = stool::LCPInterval<uint64_t>(dollerPos, dollerPos, __rlbwt.str_size());
                r.push_back(last);
                return r;
            }
        };

    } // namespace lcp_on_rlbwt
} // namespace stool