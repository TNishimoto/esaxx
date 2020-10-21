#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <vector>
#include <type_traits>
#include "../../module/rlbwt_iterator/src/include/sampling_functions.hpp"
#include "./range_distinct_on_rlbwt.hpp"
#include "./succinct_range_distinct.hpp"

namespace stool
{
    namespace lcp_on_rlbwt
    {

        template <typename INDEX_SIZE = uint64_t>
        class HyperSet
        {
        public:
            using WEINER = WeinerInterval<INDEX_SIZE>;
            std::vector<WEINER> lcpIntvVec;
            std::vector<WEINER> weinerVec;
            std::vector<uint8_t> widthVec;

            void swap(HyperSet &copy)
            {
                this->lcpIntvVec.swap(copy.lcpIntvVec);
                this->weinerVec.swap(copy.weinerVec);
                this->widthVec.swap(copy.widthVec);
            }
        };
        template <typename RLBWT_STR, typename INDEX_SIZE = uint64_t>
        class HyperSetConstructor
        {
        public:
            using CHAR = typename RLBWT_STR::char_type;
            using UCHAR = typename std::make_unsigned<CHAR>::type;
            using WEINER = WeinerInterval<INDEX_SIZE>;

            const RLBWT_STR &_rlbwt;
            std::vector<std::vector<WEINER>> tmpIntervalVec;
            std::vector<uint64_t> tmpIndexVec;
            std::vector<bool> tmpBitArray;
            std::vector<bool> checkerArray;
            SuccinctRangeDistinctDataStructure<INDEX_SIZE> srdds;
            std::vector<INDEX_SIZE> fposArray;

            uint64_t current_lcp = 0;
            uint64_t strSize = 0;
            uint64_t total_counter = 0;
            HyperSet<INDEX_SIZE> hyperSet;

            HyperSetConstructor(const RLBWT_STR &__rlbwt) : _rlbwt(__rlbwt)
            {
                    uint64_t CHARMAX = UINT8_MAX+1;
            
                uint64_t runSize = _rlbwt.rle_size();
                tmpIntervalVec.resize(CHARMAX);
                tmpIndexVec.resize(0);
                tmpBitArray.resize(CHARMAX, false);
                this->checkerArray.resize(runSize, false);
                srdds.initialize(__rlbwt);
                std::vector<INDEX_SIZE> v1 = RLBWTFunctions::construct_fpos_array<RLBWT_STR, INDEX_SIZE>(_rlbwt);
                this->fposArray.swap(v1);

                this->strSize = this->_rlbwt.str_size();
            }
            void clearWeinerTmpVec()
            {
                for (auto &it : tmpIndexVec)
                {
                    tmpBitArray[it] = false;
                    tmpIntervalVec[it].resize(0);
                }
                tmpIndexVec.resize(0);
            }
            WEINER getIntervalOnL(WEINER &interval)
            {
                WEINER output;
                INDEX_SIZE begin_pos = this->fposArray[interval.beginIndex] + interval.beginDiff;
                output.beginIndex = _rlbwt.get_lindex_containing_the_position(begin_pos);
                output.beginDiff = begin_pos - _rlbwt.get_lpos(output.beginIndex);

                INDEX_SIZE end_pos = this->fposArray[interval.endIndex] + interval.endDiff;
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
                WEINER frontL = this->getIntervalOnL(w);
                std::vector<CHAR> charOutputVec;
                vector<WEINER> results = RangeDistinctDataStructureOnRLBWT<RLBWT_STR,
                                                                           INDEX_SIZE, SuccinctRangeDistinctDataStructure<INDEX_SIZE>>::range_distinct(_rlbwt,
                                                                                                                                           srdds, frontL.beginIndex, frontL.beginDiff,
                                                                                                                                           frontL.endIndex, frontL.endDiff, charOutputVec);
                for (uint64_t i = 0; i < results.size(); i++)
                {
                    UCHAR c = charOutputVec[i];
                    auto &it = results[i];
                    bool skip = false;
                    if (isWeiner)
                    {
                        skip = !checkWeinerInterval(it);
                    }
                    if (!skip)
                    {
                        if (!tmpBitArray[c])
                        {
                            tmpIndexVec.push_back(c);
                            tmpBitArray[c] = true;
                        }
                        this->tmpIntervalVec[c].push_back(it);
                    }

                    //this->weinerTmpVec[c].push_back(intervals[i]);
                }
            }

            void computeNextSet(WEINER &lcpIntv, std::vector<WEINER> &weinerVec, uint64_t rank, HyperSet<INDEX_SIZE> &outputSet)
            {
                clearWeinerTmpVec();
                computeNextIntervals(lcpIntv, false);

                uint64_t i = rank;

                INDEX_SIZE begin_pos1 = this->fposArray[lcpIntv.beginIndex] + lcpIntv.beginDiff;
                INDEX_SIZE end_pos1 = this->fposArray[lcpIntv.endIndex] + lcpIntv.endDiff;

                while (i < weinerVec.size())
                {
                    INDEX_SIZE begin_pos2 = this->fposArray[weinerVec[i].beginIndex] + weinerVec[i].beginDiff;
                    INDEX_SIZE end_pos2 = this->fposArray[weinerVec[i].endIndex] + weinerVec[i].endDiff;
                    //lcpIntv.print();
                    //weinerVec[i].print();
                    //std::cout << std::endl;
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

                for (auto &it : tmpIndexVec)
                {
                    auto &currentVec = tmpIntervalVec[it];
                    if (currentVec.size() > 1)
                    {
                        outputSet.lcpIntvVec.push_back(currentVec[0]);
                        outputSet.widthVec.push_back(currentVec.size() - 1);
                        for (uint64_t j = 1; j < currentVec.size(); j++)
                        {
                            outputSet.weinerVec.push_back(currentVec[j]);
                        }
                    }
                }
            }
            HyperSet<INDEX_SIZE> computeFirstSet()
            {
                HyperSet<INDEX_SIZE> output;
                WEINER lcpIntv;
                auto iter = std::min_element(this->fposArray.begin(), this->fposArray.end());
                size_t minIndex = std::distance(this->fposArray.begin(), iter);
                auto iter2 = std::max_element(this->fposArray.begin(), this->fposArray.end());
                size_t maxIndex = std::distance(this->fposArray.begin(), iter2);
                uint64_t diff = _rlbwt.get_run(maxIndex) - 1;
                lcpIntv.beginIndex = minIndex;
                lcpIntv.beginDiff = 0;
                lcpIntv.endIndex = maxIndex;
                lcpIntv.endDiff = diff;
                output.lcpIntvVec.push_back(lcpIntv);

                uint64_t begin_lindex = 0;
                uint64_t begin_diff = 0;
                uint64_t end_lindex = _rlbwt.rle_size() - 1;
                uint64_t end_diff = _rlbwt.get_run(end_lindex) - 1;

                std::vector<CHAR> charOutputVec;
                auto weinerIntervals = RangeDistinctDataStructureOnRLBWT<RLBWT_STR, INDEX_SIZE, SuccinctRangeDistinctDataStructure<INDEX_SIZE>>::range_distinct(_rlbwt, srdds, begin_lindex, begin_diff, end_lindex, end_diff, charOutputVec);
                for (auto &it : weinerIntervals)
                {
                    if (checkWeinerInterval(it))
                    {
                        output.weinerVec.push_back(it);
                    }
                }
                output.widthVec.push_back(output.weinerVec.size());
                return output;
            }

            HyperSet<INDEX_SIZE> computeNextSet(HyperSet<INDEX_SIZE> &inputSet)
            {
                HyperSet<INDEX_SIZE> output;

                uint64_t rank = 0;
                for (uint64_t i = 0; i < inputSet.lcpIntvVec.size(); i++)
                {
                    computeNextSet(inputSet.lcpIntvVec[i], inputSet.weinerVec, rank, output);
                    rank += inputSet.widthVec[i];
                }
                return output;
            }
            void process()
            {
                if (current_lcp == 0)
                {
                    auto tmp = this->computeFirstSet();
                    hyperSet.swap(tmp);
                }
                else
                {
                    auto tmp = this->computeNextSet(hyperSet);
                    hyperSet.swap(tmp);
                }
                total_counter += hyperSet.weinerVec.size();
                current_lcp++;
                assert(hyperSet.weinerVec.size() > 0);
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

            static std::vector<uint64_t> constructLCPArray(const RLBWT_STR &__rlbwt)
            {
                HyperSetConstructor<RLBWT_STR, uint64_t> hsc(__rlbwt);
                std::vector<uint64_t> r;
                r.resize(hsc.strSize, 0);

                while (!hsc.isStop())
                {
                    hsc.process();
                    for (auto it : hsc.hyperSet.weinerVec)
                    {
                        uint64_t pos = hsc.fposArray[it.endIndex] + it.endDiff + 1;
                        if (pos < r.size())
                        {
                            assert(r[pos] == 0);
                            r[pos] = hsc.current_lcp - 1;
                        }
                    }
                }
                return r;
            }
            static std::vector<stool::LCPInterval<uint64_t>> constructLCPIntervals(const RLBWT_STR &__rlbwt)
            {
                HyperSetConstructor<RLBWT_STR, uint64_t> hsc(__rlbwt);
                std::vector<stool::LCPInterval<uint64_t>> r;

                while (!hsc.isStop())
                {
                    hsc.process();
                    for (auto it : hsc.hyperSet.lcpIntvVec)
                    {
                        uint64_t beg = hsc.fposArray[it.beginIndex] + it.beginDiff;
                        uint64_t end = hsc.fposArray[it.endIndex] + it.endDiff;
                        r.push_back(stool::LCPInterval<uint64_t>(beg, end, hsc.current_lcp - 1));
                    }
                }
                return r;

                //return weiner.enumerateLCPInterval();
            }

            static uint64_t outputMaximalSubstrings(const RLBWT_STR &__rlbwt, std::ofstream &out)
            {
                HyperSetConstructor<RLBWT_STR, uint64_t> hsc(__rlbwt);
                uint64_t count = 0;

                while (!hsc.isStop())
                {
                    hsc.process();
                    if(hsc.current_lcp % 100 == 0){
                    std::cout << "LCP = " << (hsc.current_lcp -1) << ", LCP Interval count = " << hsc.hyperSet.lcpIntvVec.size() << std::endl;

                    }
                    for (auto it : hsc.hyperSet.lcpIntvVec)
                    {
                        if (hsc.checkMaximalRepeat(it) && hsc.current_lcp > 1)
                        {
                            uint64_t beg = hsc.fposArray[it.beginIndex] + it.beginDiff;
                            uint64_t end = hsc.fposArray[it.endIndex] + it.endDiff;
                            stool::LCPInterval<uint64_t> newLCPIntv(beg, end, hsc.current_lcp - 1);
                            count++;
                            out.write(reinterpret_cast<const char *>(&newLCPIntv), sizeof(stool::LCPInterval<INDEX_SIZE>));
                        }
                    }

                }
                uint64_t dx = hsc.srdds.wt.select(1, 0) - 1;
                uint64_t dollerPos = __rlbwt.get_lpos(dx);
                auto last = stool::LCPInterval<INDEX_SIZE>(dollerPos, dollerPos, __rlbwt.str_size());
                out.write(reinterpret_cast<const char *>(&last), sizeof(stool::LCPInterval<INDEX_SIZE>));
                count += 1;
                return count;
            }
            static std::vector<stool::LCPInterval<uint64_t>> computeMaximalSubstrings(const RLBWT_STR &__rlbwt)
            {
                HyperSetConstructor<RLBWT_STR, uint64_t> hsc(__rlbwt);
                std::vector<stool::LCPInterval<uint64_t>> r;
                uint64_t count = 0;

                while (!hsc.isStop())
                {
                    hsc.process();
                    if(hsc.current_lcp % 100 == 0){
                    std::cout << "LCP = " << (hsc.current_lcp -1) << ", LCP Interval count = " << hsc.hyperSet.lcpIntvVec.size() << std::endl;

                    }
                    for (auto it : hsc.hyperSet.lcpIntvVec)
                    {
                        if (hsc.checkMaximalRepeat(it))
                        {
                            uint64_t beg = hsc.fposArray[it.beginIndex] + it.beginDiff;
                            uint64_t end = hsc.fposArray[it.endIndex] + it.endDiff;
                            stool::LCPInterval<uint64_t> newLCPIntv(beg, end, hsc.current_lcp - 1);
                            r.push_back(newLCPIntv);
                        }
                    }

                }
                uint64_t dx = hsc.srdds.wt.select(1, 0) - 1;
                uint64_t dollerPos = __rlbwt.get_lpos(dx);
                auto last = stool::LCPInterval<INDEX_SIZE>(dollerPos, dollerPos, __rlbwt.str_size());
                r.push_back(last);
                return r;
            }
        };

    } // namespace lcp_on_rlbwt
} // namespace stool