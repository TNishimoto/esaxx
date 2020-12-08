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

//#include "../weiner_interval.hpp"
#include "range_distinct.hpp"
#include "succinct_range_distinct.hpp"
#include "./light_range_distinct.hpp"

namespace stool
{
    namespace lcp_on_rlbwt
    {
        template <typename INDEX_SIZE, typename LPOSARRAY>
        class RangeDistinctDataStructureOnRLBWT
        {
        public:

            using RINTERVAL = RInterval<INDEX_SIZE>;
            LightRangeDistinctDataStructure<sdsl::int_vector<>, INDEX_SIZE> light_srdds;
            SuccinctRangeDistinctDataStructure<INDEX_SIZE> srdds;
            uint64_t total_cover1 = 0;
            uint64_t num1 = 0;

            uint64_t total_cover2 = 0;
            uint64_t num2 = 0;
            std::vector<CharInterval<INDEX_SIZE>> charIntervalTmpVec;
            std::vector<uint8_t> charTmpVec;

            vector<RINTERVAL> rIntervalTmpVec;
            const LPOSARRAY *lpos_array;

            //RangeDistinctDataStructureOnRLBWT(const sdsl::wt_huff<> *_wt, const sdsl::int_vector<> *_bwt, const LPOSARRAY *_lpos_array)

            void initialize(const sdsl::wt_huff<> *_wt, const sdsl::int_vector<> *_bwt, const LPOSARRAY *_lpos_array)
            {

                lpos_array = _lpos_array;
                uint64_t CHARMAX = UINT8_MAX + 1;
                charTmpVec.resize(CHARMAX);
                rIntervalTmpVec.resize(CHARMAX);
                charIntervalTmpVec.resize(CHARMAX);
                light_srdds.preprocess(_bwt);
                srdds.initialize(_wt, _bwt);
            }

            bool check(RInterval<INDEX_SIZE> &range)
            {
                uint64_t CHARMAX = UINT8_MAX + 1;

                std::vector<CharInterval<INDEX_SIZE>> DEBUGcharIntervalTmpVec1;
                std::vector<CharInterval<INDEX_SIZE>> DEBUGcharIntervalTmpVec2;
                DEBUGcharIntervalTmpVec1.resize(CHARMAX);
                DEBUGcharIntervalTmpVec2.resize(CHARMAX);

                assert(range.beginIndex <= range.endIndex);
                uint64_t count1 = srdds.range_distinct(range.beginIndex, range.endIndex, DEBUGcharIntervalTmpVec1);
                uint64_t count2 = light_srdds.range_distinct(range.beginIndex, range.endIndex, DEBUGcharIntervalTmpVec2);
                if(count1 != count2){
                    std::cout << "count distinct" << std::endl;
                    throw -1;
                }else{
                    DEBUGcharIntervalTmpVec1.resize(count1);
                    DEBUGcharIntervalTmpVec2.resize(count2);
                    stool::beller::check(DEBUGcharIntervalTmpVec1, DEBUGcharIntervalTmpVec2);

                }
                return true;

            }
            uint64_t range_distinct(RInterval<INDEX_SIZE> &range)
            {
                uint64_t count = 0;
                assert(range.beginIndex <= range.endIndex);
                //assert(check(range));

                if (range.endIndex - range.beginIndex >= 16)
                {

                    count = srdds.range_distinct(range.beginIndex, range.endIndex, charIntervalTmpVec);

                    total_cover1 += range.endIndex - range.beginIndex + 1;
                    num1++;
                }
                else
                {
                    count = light_srdds.range_distinct(range.beginIndex, range.endIndex, charIntervalTmpVec);
                    //std::cout << "+" << count << std::endl;

                    total_cover2 += range.endIndex - range.beginIndex + 1;
                    num2++;
                }

                assert(count > 0);

                for (uint64_t x = 0; x < count; x++)
                {
                    auto &it = charIntervalTmpVec[x];
                    INDEX_SIZE cBeginIndex = it.i;
                    INDEX_SIZE cEndIndex = it.j;
                    INDEX_SIZE cBeginDiff = cBeginIndex == range.beginIndex ? range.beginDiff : 0;
                    uint64_t end_run = (*lpos_array)[cEndIndex + 1] - (*lpos_array)[cEndIndex];
                    INDEX_SIZE cEndDiff = cEndIndex == range.endIndex ? range.endDiff : end_run - 1;

                    RInterval<INDEX_SIZE> cInterval;
                    cInterval.beginIndex = cBeginIndex;
                    cInterval.beginDiff = cBeginDiff;
                    cInterval.endIndex = cEndIndex;
                    cInterval.endDiff = cEndDiff;

                    charTmpVec[x] = it.c;
                    rIntervalTmpVec[x] = cInterval;
                }

                return count;
            }
        };

    } // namespace lcp_on_rlbwt
} // namespace stool