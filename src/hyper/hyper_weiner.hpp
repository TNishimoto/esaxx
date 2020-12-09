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

#include "parallel_stnode_wtraverser.hpp"
#include "weiner_link_emulator.hpp"
#include <thread>

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
            ParallelSTNodeWTraverser<INDEX_SIZE, RLBWTDS> stnodeSequencer;
            /*
            STNodeWTraverser<INDEX_SIZE, RLBWTDS> hyperSet;
            STNodeWTraverser<INDEX_SIZE, RLBWTDS> hyperTmpSet;
            */

            //uint64_t current_lcp = 0;
            uint64_t debugCounter = 0;
            //ExplicitWeinerLinkEmulator<INDEX_SIZE, RLBWTDS> em;

            HyperSetConstructor(RLBWTDS &__RLBWTDS, uint64_t thread_num) : _RLBWTDS(__RLBWTDS)
            {
                //lightWeight = _lightWeight;
                //uint64_t runSize = __rlbwt.rle_size();
                //this->wds.initialize(&this->_RLBWTDS);

                //em.initialize(&this->_RLBWTDS);

                stnodeSequencer.initialize(thread_num, __RLBWTDS);

                //srdds.initialize(__rlbwt);

                //charIntervalTmpVec.resize(CHARMAX);
            }

            /*
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
            */
            bool checkMaximalRepeat(const RINTERVAL &lcpIntv)
            {
                RINTERVAL it = this->_RLBWTDS.getIntervalOnL(lcpIntv);
                uint8_t fstChar = this->_RLBWTDS.get_char_by_run_index(it.beginIndex);
                uint8_t lstChar = this->_RLBWTDS.get_char_by_run_index(it.endIndex);
                if (fstChar == lstChar)
                {

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

            static std::vector<uint64_t> constructLCPArray(RLBWTDS *__RLBWTDS, int thread_num)
            {
                HyperSetConstructor<RLBWTDS> hsc(__RLBWTDS, thread_num);
                std::vector<uint64_t> r;
                r.resize(hsc.stnodeSequencer.strSize, 0);

                while (!hsc.stnodeSequencer.isStop())
                {
                    hsc.stnodeSequencer.process();
                    for (uint64_t i = 0; i < hsc.stnodeSequencer.child_count; i++)
                    {
                        auto &it = hsc.stnodeSequencer.get_child(i);
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
            static std::vector<stool::LCPInterval<uint64_t>> constructLCPIntervals(RLBWTDS *__RLBWTDS, int thread_num)
            {
                HyperSetConstructor<RLBWTDS> hsc(*__RLBWTDS, thread_num);

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

            static uint64_t outputMaximalSubstrings(std::ofstream &out, RLBWTDS *__RLBWTDS, int thread_num)
            {
                HyperSetConstructor<RLBWTDS> hsc(*__RLBWTDS, thread_num);

                uint64_t count = 0;

                while (!hsc.stnodeSequencer.isStop())
                {

                    hsc.stnodeSequencer.process();
                    /*
                    if (hsc.current_lcp % 100 == 0)
                    {
                        std::cout << "LCP = " << (hsc.current_lcp - 1) << ", LCP Interval count = " << hsc.hyperSet.lcpIntvCount << std::endl;
                    }
                    */
                    auto start = std::chrono::system_clock::now();

                    for (uint64_t i = 0; i < hsc.stnodeSequencer.node_count; i++)
                    {
                        auto &it = hsc.stnodeSequencer.get_stnode(i);
                        /*
                        if (hsc.checkMaximalRepeat(it))
                        {
                            
                            uint64_t beg = hsc._RLBWTDS.get_fpos(it.beginIndex, it.beginDiff);
                            uint64_t end = hsc._RLBWTDS.get_fpos(it.endIndex, it.endDiff);
                            stool::LCPInterval<uint64_t> newLCPIntv(beg, end, hsc.stnodeSequencer.current_lcp - 1);
                            count++;
                            out.write(reinterpret_cast<const char *>(&newLCPIntv), sizeof(stool::LCPInterval<INDEX_SIZE>));
                            
                        }
                        */
                    }
                    auto end = std::chrono::system_clock::now();
                    double elapsed1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                    std::cout << "Loop Time: " << elapsed1 << std::endl;
                }
                /*
                double average = ((double)(hsc._RLBWTDS.rangeOnRLBWT.total_cover1 + hsc._RLBWTDS.rangeOnRLBWT.total_cover2)) / ((double)(hsc._RLBWTDS.rangeOnRLBWT.num1 + hsc._RLBWTDS.rangeOnRLBWT.num2));
                std::cout << "@" << hsc._RLBWTDS.rangeOnRLBWT.total_cover1 << "/" << hsc._RLBWTDS.rangeOnRLBWT.num1 << std::endl;
                std::cout << "@" << hsc._RLBWTDS.rangeOnRLBWT.total_cover2 << "/" << hsc._RLBWTDS.rangeOnRLBWT.num2 << std::endl;
                std::cout << "@Average: " << average << std::endl;

                std::cout << "Range Distinct Count = " << hsc.debugCounter << "/" << __RLBWTDS->str_size()  << std::endl;
                */

                uint64_t dx = __RLBWTDS->get_end_rle_lposition();
                uint64_t dollerPos = __RLBWTDS->get_lpos(dx);
                auto last = stool::LCPInterval<INDEX_SIZE>(dollerPos, dollerPos, __RLBWTDS->str_size());
                out.write(reinterpret_cast<const char *>(&last), sizeof(stool::LCPInterval<INDEX_SIZE>));
                count += 1;
                return count;
            }
            static std::vector<stool::LCPInterval<uint64_t>> computeMaximalSubstrings(RLBWTDS *__RLBWTDS, int thread_num)
            {
                HyperSetConstructor<RLBWTDS> hsc(*__RLBWTDS, thread_num);

                std::vector<stool::LCPInterval<uint64_t>> r;

                while (!hsc.stnodeSequencer.isStop())
                {
                    hsc.stnodeSequencer.process();

                    if (hsc.stnodeSequencer.current_lcp % 100 == 0)
                    {
                        std::cout << "LCP = " << (hsc.stnodeSequencer.current_lcp - 1) << ", LCP Interval count = " << hsc.stnodeSequencer.node_count << std::endl;
                    }
                    for (uint64_t i = 0; i < hsc.stnodeSequencer.node_count; i++)
                    {
                        auto &it = hsc.stnodeSequencer.get_stnode(i);
                        if (hsc.checkMaximalRepeat(it))
                        {
                            uint64_t beg = hsc._RLBWTDS.get_fpos(it.beginIndex, it.beginDiff);
                            uint64_t end = hsc._RLBWTDS.get_fpos(it.endIndex, it.endDiff);
                            stool::LCPInterval<uint64_t> newLCPIntv(beg, end, hsc.stnodeSequencer.current_lcp - 1);
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