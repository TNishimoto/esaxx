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

namespace stool
{
    namespace lcp_on_rlbwt
    {
        template <typename RLBWT_STR, typename INDEX_SIZE>
        class RangeDistinctDataStructureOnRLBWT
        {
        public:
            using CHAR = typename RLBWT_STR::char_type;
            using CHAR_VEC = typename RLBWT_STR::char_vec_type;
            using RINTERVAL = RInterval<INDEX_SIZE>;

            const RLBWT_STR *rlbwt;
            //RANGE_DISTINCT *rd
            //RangeDistinctDataStructure<CHAR_VEC, INDEX_SIZE> srdds2;
            LightRangeDistinctDataStructure<CHAR_VEC, INDEX_SIZE> light_srdds;
            SuccinctRangeDistinctDataStructure<INDEX_SIZE> srdds;


            uint64_t total_cover1 = 0;
            uint64_t num1 = 0;

            uint64_t total_cover2 = 0;
            uint64_t num2 = 0;

            //std::vector<RInterval<INDEX_SIZE>> &output

            std::vector<CharInterval<INDEX_SIZE>> charIntervalTmpVec;
            std::vector<CHAR> charTmpVec;
            vector<RINTERVAL> rIntervalTmpVec;


            void initialize(const RLBWT_STR *_rlbwt, const sdsl::wt_huff<> *_wt, const sdsl::int_vector<> *_bwt)
            {
                uint64_t CHARMAX = UINT8_MAX + 1;
                charTmpVec.resize(CHARMAX);
                rIntervalTmpVec.resize(CHARMAX);


                this->rlbwt = _rlbwt;
                //this->rd = _rd;
                charIntervalTmpVec.resize(CHARMAX);
                //srdds2.preprocess(rlbwt->get_char_vec(), _wt);
                light_srdds.preprocess(rlbwt->get_char_vec());
                srdds.initialize(_wt, _bwt);
            }

            uint64_t range_distinct(RInterval<INDEX_SIZE> range)
            {

                uint64_t count = 0;

                if (range.endIndex - range.beginIndex >= 16)
                {

                    //std::vector<CharInterval<INDEX_SIZE>> charIntervalTmpVec;

                    //count = srdds2.range_distinct(range.beginIndex, range.endIndex, charIntervalTmpVec);
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
                    INDEX_SIZE cEndDiff = cEndIndex == range.endIndex ? range.endDiff : rlbwt->get_run(cEndIndex) - 1;

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