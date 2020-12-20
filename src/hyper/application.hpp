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
        class STTreeAnalysisResult
        {
        public:
            uint64_t max_nodes_at_level;
        };

        template <typename INDEX_SIZE, typename RLBWTDS>
        void find_maximal_repeats(ParallelSTNodeWTraverser<INDEX_SIZE, RLBWTDS> &stnodeSequencer, RLBWTDS &_RLBWTDS, uint64_t start_index, uint64_t size, std::vector<bool> &ms_check_vec)
        {
            ms_check_vec.resize(size, false);
            uint64_t end_index = start_index + size - 1;
            for (uint64_t i = start_index; i <= end_index; i++)
            {
                const RInterval<INDEX_SIZE> &it = stnodeSequencer.get_stnode(i);
                bool b = checkMaximalRepeat(it, _RLBWTDS);
                ms_check_vec[i] = b;
            }
        }
        /*
        template <typename INDEX_SIZE, typename RLBWTDS>
        void find_maximal_repeats_with_multi_thread(ParallelSTNodeWTraverser<INDEX_SIZE, RLBWTDS> &stnodeSequencer, RLBWTDS &_RLBWTDS, std::vector<bool> &ms_check_vec, uint64_t )
        {
            ms_check_vec.resize(size, false);
            uint64_t end_index = start_index + size - 1;
            for(uint64_t i=start_index;i <= end_index;i++){
                const RInterval<INDEX_SIZE> &it = stnodeSequencer.get_stnode(i);
                bool b = checkMaximalRepeat(it, _RLBWTDS);
                ms_check_vec[i] = b;
            }
        }
        */

        template <typename RLBWTDS>
        class Application
        {
        public:
            using CHAR = typename RLBWTDS::CHAR;
            //using CHARVEC = typename RLBWT_STR::char_vec_type;
            using INDEX_SIZE = typename RLBWTDS::INDEX;
            using UCHAR = typename std::make_unsigned<CHAR>::type;
            using RINTERVAL = RInterval<INDEX_SIZE>;

            /*
            static std::vector<uint64_t> constructLCPArray(ParallelSTNodeWTraverser<INDEX_SIZE, RLBWTDS> &stnodeSequencer)
            {
                std::vector<uint64_t> r;
                r.resize(stnodeSequencer.strSize, 0);

                while (!stnodeSequencer.isStop())
                {
                    stnodeSequencer.process();
                    for (uint64_t i = 0; i < stnodeSequencer.child_count; i++)
                    {
                        auto &it = stnodeSequencer.get_child(i);
                        uint64_t pos = stnodeSequencer._RLBWTDS->get_fpos(it.endIndex, it.endDiff) + 1;
                        if (pos < r.size())
                        {
                            assert(r[pos] == 0);
                            r[pos] = stnodeSequencer.current_lcp - 1;
                        }
                    }
                }
                return r;
            }
            */
            static std::vector<stool::LCPInterval<uint64_t>> constructLCPIntervals(ParallelSTNodeWTraverser<INDEX_SIZE, RLBWTDS> &stnodeSequencer)
            {

                std::vector<stool::LCPInterval<uint64_t>> r;

                while (!stnodeSequencer.isStop())
                {
                    stnodeSequencer.process();
                    for (uint64_t i = 0; i < stnodeSequencer.node_count; i++)
                    {
                        auto &it = stnodeSequencer.get_stnode(i);
                        uint64_t beg = stnodeSequencer._RLBWTDS->get_fpos(it.beginIndex, it.beginDiff);
                        uint64_t end = stnodeSequencer._RLBWTDS->get_fpos(it.endIndex, it.endDiff);
                        r.push_back(stool::LCPInterval<uint64_t>(beg, end, stnodeSequencer.current_lcp - 1));
                    }
                }
                return r;

                //return weiner.enumerateLCPInterval();
            }

            static uint64_t outputMaximalSubstrings(std::ofstream &out, ParallelSTNodeWTraverser<INDEX_SIZE, RLBWTDS> &stnodeSequencer, STTreeAnalysisResult &analysis)
            {

                uint64_t count = 0;

                while (!stnodeSequencer.isStop())
                {

                    stnodeSequencer.process();
                    /*
                    if (stnodeSequencer.current_lcp % 100 == 0)
                    {
                        std::cout << "LCP = " << (stnodeSequencer.current_lcp - 1) << ", LCP Interval count = " << stnodeSequencer.total_counter << std::endl;
                    }
                    */
                    
                    auto start = std::chrono::system_clock::now();

                    auto trees = stnodeSequencer.get_sub_trees();

                    for (uint64_t i = 0; i < trees->size(); i++)
                    {
                        auto &tree = (*trees)[i];

                        assert(tree.node_count() == tree.maximal_repeat_check_vec.size());
                        for (uint64_t j = 0; j < tree.maximal_repeat_check_vec.size(); j++)
                        {
                            auto &it = tree.get_stnode(j);
                            bool b = tree.maximal_repeat_check_vec[j];
                            if (b)
                            {
                                count++;
                                /*
                                uint64_t beg = stnodeSequencer._RLBWTDS->get_fpos(it.beginIndex, it.beginDiff);
                                uint64_t end = stnodeSequencer._RLBWTDS->get_fpos(it.endIndex, it.endDiff);
                                //std::cout << beg << ", " << end << std::endl;
                                stool::LCPInterval<uint64_t> newLCPIntv(beg, end, stnodeSequencer.current_lcp - 1);

                                out.write(reinterpret_cast<const char *>(&newLCPIntv), sizeof(stool::LCPInterval<INDEX_SIZE>));
                                */
                            }
                        }
                    }
                    auto end = std::chrono::system_clock::now();
                    double elapsed1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                    /*
                    if (elapsed1 > 10)
                    {
                        std::cout << "Loop Time: " << elapsed1 << std::endl;
                    }
                    */
                }
                analysis.max_nodes_at_level = stnodeSequencer.peak_child_count;
                /*
                double average = ((double)(hsc._RLBWTDS.rangeOnRLBWT.total_cover1 + hsc._RLBWTDS.rangeOnRLBWT.total_cover2)) / ((double)(hsc._RLBWTDS.rangeOnRLBWT.num1 + hsc._RLBWTDS.rangeOnRLBWT.num2));
                std::cout << "@" << hsc._RLBWTDS.rangeOnRLBWT.total_cover1 << "/" << hsc._RLBWTDS.rangeOnRLBWT.num1 << std::endl;
                std::cout << "@" << hsc._RLBWTDS.rangeOnRLBWT.total_cover2 << "/" << hsc._RLBWTDS.rangeOnRLBWT.num2 << std::endl;
                std::cout << "@Average: " << average << std::endl;

                std::cout << "Range Distinct Count = " << hsc.debugCounter << "/" << __RLBWTDS->str_size()  << std::endl;
                */

                uint64_t dx = stnodeSequencer._RLBWTDS->get_end_rle_lposition();
                uint64_t dollerPos = stnodeSequencer._RLBWTDS->get_lpos(dx);
                auto last = stool::LCPInterval<INDEX_SIZE>(dollerPos, dollerPos, stnodeSequencer._RLBWTDS->str_size());
                out.write(reinterpret_cast<const char *>(&last), sizeof(stool::LCPInterval<INDEX_SIZE>));
                count += 1;
                return count;
            }
            static std::vector<stool::LCPInterval<uint64_t>> computeMaximalSubstrings(ParallelSTNodeWTraverser<INDEX_SIZE, RLBWTDS> &stnodeSequencer)
            {
                //Application<RLBWTDS> hsc(*__RLBWTDS, thread_num);

                std::vector<stool::LCPInterval<uint64_t>> r;

                while (!stnodeSequencer.isStop())
                {
                    stnodeSequencer.process();
                    /*
                    if (stnodeSequencer.current_lcp % 100 == 0)
                    {
                        std::cout << "LCP = " << (stnodeSequencer.current_lcp - 1) << ", LCP Interval count = " << stnodeSequencer.node_count << std::endl;
                    }
                    */
                    auto trees = stnodeSequencer.get_sub_trees();

                    for (uint64_t i = 0; i < trees->size(); i++)
                    {
                        auto &tree = (*trees)[i];

                        assert(tree.node_count() == tree.maximal_repeat_check_vec.size());
                        for (uint64_t j = 0; j < tree.maximal_repeat_check_vec.size(); j++)
                        {
                            auto &it = tree.get_stnode(j);
                            bool b = tree.maximal_repeat_check_vec[j];
                            if (b)
                            {

                                uint64_t beg = stnodeSequencer._RLBWTDS->get_fpos(it.beginIndex, it.beginDiff);
                                uint64_t end = stnodeSequencer._RLBWTDS->get_fpos(it.endIndex, it.endDiff);
                                //std::cout << beg << ", " << end << std::endl;
                                stool::LCPInterval<uint64_t> newLCPIntv(beg, end, stnodeSequencer.current_lcp - 1);
                                r.push_back(newLCPIntv);
                            }
                        }
                    }
                }
                uint64_t dx = stnodeSequencer._RLBWTDS->get_end_rle_lposition();
                uint64_t dollerPos = stnodeSequencer._RLBWTDS->get_lpos(dx);
                auto last = stool::LCPInterval<uint64_t>(dollerPos, dollerPos, stnodeSequencer._RLBWTDS->str_size());
                r.push_back(last);
                return r;
            }
        };

    } // namespace lcp_on_rlbwt
} // namespace stool