#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <vector>
#include <type_traits>
//#include "next_rinterval_storage_constructor.hpp"
#include "../../module/rlbwt_iterator/src/include/weiner/sampling_functions.hpp"
#include "./range_distinct/rlbwt_data_structures.hpp"

#include "rinterval_storage.hpp"
#include "weiner_link_emulator.hpp"


namespace stool
{
    namespace lcp_on_rlbwt
    {

        template <typename RLBWTDS>
        class HyperSetConstructor
        {
        public:
            using CHAR = typename RLBWTDS::CHAR;
            //using CHARVEC = typename RLBWT_STR::char_vec_type;
            using INDEX_SIZE = typename RLBWTDS::INDEX;
            using UCHAR = typename std::make_unsigned<CHAR>::type;
            using RINTERVAL = RInterval<INDEX_SIZE>;

            //bool lightWeight = false;

            //NextRIntervalStorageConstructor<INDEX_SIZE, RLBWTDS> wds;
            RLBWTDS &_RLBWTDS;
            RIntervalStorage<INDEX_SIZE, RLBWTDS> hyperSet;
            RIntervalStorage<INDEX_SIZE, RLBWTDS> hyperTmpSet;

            uint64_t current_lcp = 0;
            uint64_t strSize = 0;
            uint64_t total_counter = 0;
            uint64_t debugCounter = 0;
            ExplicitWeinerLinkEmulator<INDEX_SIZE, RLBWTDS> em;


            HyperSetConstructor(RLBWTDS &__RLBWTDS) : _RLBWTDS(__RLBWTDS)
            {
                //lightWeight = _lightWeight;
                //uint64_t runSize = __rlbwt.rle_size();
                //this->wds.initialize(&this->_RLBWTDS);

                em.initialize(&this->_RLBWTDS);

                //srdds.initialize(__rlbwt);

                this->strSize = this->_RLBWTDS.str_size();

                //charIntervalTmpVec.resize(CHARMAX);

            }

            

            void process()
            {
                if (current_lcp == 0)
                {
                    this->hyperSet.first_compute(em);
                }
                else
                {
                    this->hyperTmpSet.computeNextLCPIntervalSet(this->hyperSet, em);
                    //this->wds.computeNextLCPIntervalSet(this->hyperSet, this->hyperTmpSet);
                    this->hyperSet.swap(this->hyperTmpSet);
                }

                total_counter += hyperSet.weinerCount;
                assert(total_counter <= strSize);

                current_lcp++;
                assert(hyperSet.weinerCount > 0);
            }
            bool isStop()
            {
                //std::cout << "STOP?" << strSize << "/" << total_counter << std::endl;
                return total_counter == strSize;
            }
            bool checkMaximalRepeat(const RINTERVAL &lcpIntv)
            {
                RINTERVAL it = this->_RLBWTDS.getIntervalOnL(lcpIntv);
                uint8_t fstChar = this->_RLBWTDS.get_char_by_run_index(it.beginIndex);
                uint8_t lstChar = this->_RLBWTDS.get_char_by_run_index(it.endIndex);
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

            static std::vector<uint64_t> constructLCPArray(RLBWTDS *__RLBWTDS)
            {
                HyperSetConstructor<RLBWTDS> hsc(__RLBWTDS);
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
            static std::vector<stool::LCPInterval<uint64_t>> constructLCPIntervals(RLBWTDS *__RLBWTDS)
            {                
                HyperSetConstructor<RLBWTDS> hsc(*__RLBWTDS);

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

            static uint64_t outputMaximalSubstrings(std::ofstream &out, RLBWTDS *__RLBWTDS)
            {
                HyperSetConstructor<RLBWTDS> hsc(*__RLBWTDS);

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

                std::cout << "Range Distinct Count = " << hsc.debugCounter << "/" << __RLBWTDS->str_size()  << std::endl;

                uint64_t dx = __RLBWTDS->get_end_rle_lposition();
                uint64_t dollerPos = __RLBWTDS->get_lpos(dx);
                auto last = stool::LCPInterval<INDEX_SIZE>(dollerPos, dollerPos, __RLBWTDS->str_size());
                out.write(reinterpret_cast<const char *>(&last), sizeof(stool::LCPInterval<INDEX_SIZE>));
                count += 1;
                return count;
            }
            static std::vector<stool::LCPInterval<uint64_t>> computeMaximalSubstrings(RLBWTDS *__RLBWTDS)
            {
                HyperSetConstructor<RLBWTDS> hsc(*__RLBWTDS);

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
                uint64_t dx = __RLBWTDS->get_end_rle_lposition();
                uint64_t dollerPos = __RLBWTDS->get_lpos(dx);
                auto last = stool::LCPInterval<uint64_t>(dollerPos, dollerPos, __RLBWTDS->str_size());
                r.push_back(last);
                return r;
            }
        };

    } // namespace lcp_on_rlbwt
} // namespace stool