#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include "rlbwt_iterator/src/include/rlbwt.hpp"
#include "rlbwt_iterator/src/include/backward_text.hpp"
#include "rlbwt_iterator/src/include/rle_farray.hpp"
#include "rlbwt_iterator/src/include/rlbwt_functions.hpp"

#include "range_distinct.hpp"
#include "succinct_range_distinct.hpp"

namespace stool
{
    namespace lcp_on_rlbwt
    {
        template <typename INDEX_SIZE>
        struct WeinerInterval
        {
            INDEX_SIZE beginIndex;
            INDEX_SIZE beginDiff;
            INDEX_SIZE endIndex;
            INDEX_SIZE endDiff;

            void print()
            {
                std::cout << "[" << this->beginIndex << ", " << this->beginDiff << ", " << this->endIndex << ", " << this->endDiff << "]" << std::endl;
            }
            void print2(std::vector<INDEX_SIZE> &fposArray)
            {
                if (this->is_special())
                {
                    std::cout << "[BOTTOM]" << std::endl;
                }
                else
                {
                    INDEX_SIZE begin_pos = fposArray[this->beginIndex] + this->beginDiff;
                    INDEX_SIZE end_pos = fposArray[this->endIndex] + this->endDiff;

                    std::cout << "[" << begin_pos << ", " << end_pos << "]" << std::endl;
                }
            }

            bool is_special()
            {
                return this->beginIndex == std::numeric_limits<INDEX_SIZE>::max();
            }

            static WeinerInterval get_special()
            {
                WeinerInterval r;
                r.beginIndex = std::numeric_limits<INDEX_SIZE>::max();
                return r;
            }
        };

        template <typename RLBWT_STR, typename INDEX_SIZE, typename RANGE_DISTINCT>
        class RangeDistinctDataStructureOnRLBWT
        {
        public:
            using CHAR = typename RLBWT_STR::char_type;
            using CHAR_VEC = typename RLBWT_STR::char_vec_type;
            static std::vector<WeinerInterval<INDEX_SIZE>> range_distinct(const RLBWT_STR &_rlbwt,
                                                                          RANGE_DISTINCT &rd, INDEX_SIZE &begin_lindex, INDEX_SIZE &begin_diff, INDEX_SIZE &end_lindex, INDEX_SIZE &end_diff, std::vector<CHAR> &charOutputVec)
            {

                vector<WeinerInterval<INDEX_SIZE>> r;

                vector<stool::CharInterval> rangeVec = rd.range_distinct(begin_lindex, end_lindex);

                for (auto &it : rangeVec)
                {
                    //CHAR c = _rlbwt.get_char_by_run_index(it.first);
                    INDEX_SIZE cBeginIndex = it.i;
                    INDEX_SIZE cEndIndex = it.j;
                    INDEX_SIZE cBeginDiff = cBeginIndex == begin_lindex ? begin_diff : 0;
                    INDEX_SIZE cEndDiff = cEndIndex == end_lindex ? end_diff : _rlbwt.get_run(cEndIndex) - 1;

                    WeinerInterval<INDEX_SIZE> cInterval;
                    cInterval.beginIndex = cBeginIndex;
                    cInterval.beginDiff = cBeginDiff;
                    cInterval.endIndex = cEndIndex;
                    cInterval.endDiff = cEndDiff;

                    charOutputVec.push_back(it.c);

                    r.push_back(cInterval);
                }
                return r;
            }

            static bool equal_check(std::vector<stool::CharInterval> &item1, std::vector<stool::CharInterval> &item2)
            {

                sort(item1.begin(), item1.end(), [](const stool::CharInterval &lhs, const stool::CharInterval &rhs) {
                    if (lhs.c != rhs.c)
                    {
                        return lhs.c < rhs.c;
                    }
                    else
                    {
                        return lhs.i < rhs.i;
                    }
                });

                sort(item2.begin(), item2.end(), [](const stool::CharInterval &lhs, const stool::CharInterval &rhs) {
                    if (lhs.c != rhs.c)
                    {
                        return lhs.c < rhs.c;
                    }
                    else
                    {
                        return lhs.i < rhs.i;
                    }
                });
                /*
                if (item1.size() < 100)
                {
                    std::cout << "Test Vec:" << std::endl;
                    for (auto it : item1)
                    {
                        std::cout << it.to_string();
                    }
                    std::cout << std::endl;

                    std::cout << "Correct Vec:" << std::endl;
                    for (auto it : item2)
                    {
                        std::cout << it.to_string();
                    }
                    std::cout << std::endl;
                }
                */

                if (item1.size() != item2.size())
                {
                    std::cout << "Distinct Size!" << item1.size() << "/" << item2.size() << std::endl;
                    throw -1;
                }
                else
                {
                    for (uint64_t i = 0; i < item1.size(); i++)
                    {

                        if (item1[i].i != item2[i].i || item1[i].j != item2[i].j || item1[i].c != item2[i].c)
                        {
                            std::cout << "Distinct Value!" << std::endl;

                            throw -1;
                        }
                    }
                }
                return true;
            }

            static std::vector<WeinerInterval<INDEX_SIZE>> range_distinct2(const RLBWT_STR &_rlbwt,
                                                                           SuccinctRangeDistinctDataStructure &rd1, RangeDistinctDataStructure<CHAR_VEC, INDEX_SIZE> &rd2, INDEX_SIZE &begin_lindex, INDEX_SIZE &begin_diff, INDEX_SIZE &end_lindex, INDEX_SIZE &end_diff, std::vector<CHAR> &charOutputVec)
            {

                vector<WeinerInterval<INDEX_SIZE>> r;

                vector<stool::CharInterval> rangeVec = rd1.range_distinct(begin_lindex, end_lindex);
                vector<stool::CharInterval> rangeVec2 = rd2.range_distinct(begin_lindex, end_lindex);
                equal_check(rangeVec, rangeVec2);

                for (auto &it : rangeVec)
                {
                    //CHAR c = _rlbwt.get_char_by_run_index(it.first);
                    INDEX_SIZE cBeginIndex = it.i;
                    INDEX_SIZE cEndIndex = it.j;
                    INDEX_SIZE cBeginDiff = cBeginIndex == begin_lindex ? begin_diff : 0;
                    INDEX_SIZE cEndDiff = cEndIndex == end_lindex ? end_diff : _rlbwt.get_run(cEndIndex) - 1;

                    WeinerInterval<INDEX_SIZE> cInterval;
                    cInterval.beginIndex = cBeginIndex;
                    cInterval.beginDiff = cBeginDiff;
                    cInterval.endIndex = cEndIndex;
                    cInterval.endDiff = cEndDiff;

                    charOutputVec.push_back(it.c);

                    r.push_back(cInterval);
                }
                return r;
            }
        };

    } // namespace lcp_on_rlbwt
} // namespace stool