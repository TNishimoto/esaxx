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
#include "./rlbwt_data_structures.hpp"

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
            using RINTERVAL = RInterval<INDEX_SIZE>;

            bool lightWeight = false;

            WeinerDataStructures<INDEX_SIZE, CHAR> wds;
            RLBWTDataStructures<RLBWT_STR, INDEX_SIZE> _RLBWTDS;
            HyperSet<INDEX_SIZE> hyperSet;
            HyperSet<INDEX_SIZE> hyperTmpSet;

            uint64_t current_lcp = 0;
            uint64_t strSize = 0;
            uint64_t total_counter = 0;
            uint64_t debugCounter = 0;


            HyperSetConstructor(const RLBWT_STR &__rlbwt, bool _lightWeight) : _RLBWTDS(__rlbwt, _lightWeight)
            {
                lightWeight = _lightWeight;
                uint64_t runSize = __rlbwt.rle_size();
                this->wds.initialize(runSize);


                //srdds.initialize(__rlbwt);

                this->strSize = this->_RLBWTDS._rlbwt.str_size();

                //charIntervalTmpVec.resize(CHARMAX);

            }

            

            void process()
            {
                if (current_lcp == 0)
                {
                    this->_RLBWTDS.computeFirstLCPIntervalSet(this->hyperTmpSet, wds);
                    this->hyperSet.swap(this->hyperTmpSet);
                }
                else
                {
                    this->_RLBWTDS.computeNextLCPIntervalSet(this->hyperSet, this->hyperTmpSet, wds);
                    this->hyperSet.swap(this->hyperTmpSet);
                }

                total_counter += hyperSet.weinerCount;
                current_lcp++;
                assert(hyperSet.weinerCount > 0);
            }
            bool isStop()
            {
                return strSize == total_counter;
            }
            bool checkMaximalRepeat(const RINTERVAL &lcpIntv)
            {
                RINTERVAL it = this->_RLBWTDS.getIntervalOnL(lcpIntv);
                uint8_t fstChar = this->_RLBWTDS._rlbwt.get_char_by_run_index(it.beginIndex);
                uint8_t lstChar = this->_RLBWTDS._rlbwt.get_char_by_run_index(it.endIndex);
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
                        uint64_t beg = hsc._RLBWTDS.get_fpos(it.beginIndex, it.beginDiff);
                        uint64_t end = hsc._RLBWTDS.get_fpos(it.endIndex, it.endDiff);
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
                            uint64_t beg = hsc._RLBWTDS.get_fpos(it.beginIndex, it.beginDiff);
                            uint64_t end = hsc._RLBWTDS.get_fpos(it.endIndex, it.endDiff);
                            stool::LCPInterval<uint64_t> newLCPIntv(beg, end, hsc.current_lcp - 1);
                            count++;
                            out.write(reinterpret_cast<const char *>(&newLCPIntv), sizeof(stool::LCPInterval<INDEX_SIZE>));
                        }
                    }
                }
                //double px = hsc.rangeOnRLBWT.total_cover / hsc.rangeOnRLBWT.num;

                double average = ((double)(hsc._RLBWTDS.rangeOnRLBWT.total_cover1 + hsc._RLBWTDS.rangeOnRLBWT.total_cover2)) / ((double)(hsc._RLBWTDS.rangeOnRLBWT.num1 + hsc._RLBWTDS.rangeOnRLBWT.num2));
                std::cout << "@" << hsc._RLBWTDS.rangeOnRLBWT.total_cover1 << "/" << hsc._RLBWTDS.rangeOnRLBWT.num1 << std::endl;
                std::cout << "@" << hsc._RLBWTDS.rangeOnRLBWT.total_cover2 << "/" << hsc._RLBWTDS.rangeOnRLBWT.num2 << std::endl;
                std::cout << "@Average: " << average << std::endl;

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
                            uint64_t beg = hsc._RLBWTDS.get_fpos(it.beginIndex, it.beginDiff);
                            uint64_t end = hsc._RLBWTDS.get_fpos(it.endIndex, it.endDiff);
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