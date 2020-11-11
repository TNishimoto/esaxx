#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <vector>
#include <type_traits>
#include "../../module/rlbwt_iterator/src/include/weiner/sampling_functions.hpp"
#include "./range_distinct/rlbwt_data_structures.hpp"
#include "rinterval_storage.hpp"
#include "rinterval_temporary_storage.hpp"

//#include "../../module/rlbwt_iterator/src/include/weiner/sampling_functions.hpp"
//#include "range_distinct/range_distinct_on_rlbwt.hpp"

namespace stool
{
    namespace lcp_on_rlbwt
    {

        template <typename RLBWT_STR, typename INDEX_SIZE>
        class NextRIntervalStorageConstructor
        {

            using CHAR = typename RLBWT_STR::char_type;

            using RINTERVAL = RInterval<INDEX_SIZE>;
            using UCHAR = typename std::make_unsigned<CHAR>::type;

            std::vector<bool> checkerArray;
            RLBWTDataStructures<RLBWT_STR, INDEX_SIZE> *_RLBWTDS;
            RIntervalTemporaryStorage<INDEX_SIZE> intervalTemporary;

            bool checkWeinerInterval(RINTERVAL &w)
            {
                bool b = this->_RLBWTDS->_rlbwt.get_run(w.endIndex) == (w.endDiff + 1);
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

        public:
            void initialize(RLBWTDataStructures<RLBWT_STR, INDEX_SIZE> *_rlbwtds)
            {
                this->_RLBWTDS = _rlbwtds;
                uint64_t runSize = _rlbwtds->_rlbwt.rle_size();
                //uint64_t CHARMAX = UINT8_MAX + 1;
                this->checkerArray.resize(runSize, false);
                intervalTemporary.initialize();
            }

        private:
            void computeNextIntervals(const RINTERVAL &w, bool isWeiner, RIntervalTemporaryStorage<INDEX_SIZE> &output)
            {
                RINTERVAL frontL = this->_RLBWTDS->getIntervalOnL(w);
                uint64_t resultCount = this->_RLBWTDS->rangeOnRLBWT.range_distinct(frontL);

                for (uint64_t i = 0; i < resultCount; i++)
                {
                    UCHAR c = this->_RLBWTDS->rangeOnRLBWT.charTmpVec[i];
                    auto &it = this->_RLBWTDS->rangeOnRLBWT.rIntervalTmpVec[i];
                    if (isWeiner)
                    {
                        if(this->checkWeinerInterval(it)){
                            output.pushWeinerInterval(it, c);
                        }
                    }
                    else
                    {
                        if(output.occur(c)){
                            output.pushLCPInterval(it, c);
                        }
                    }
                }
            }
            void computeNextLCPIntervalSet(const RINTERVAL &lcpIntv, const std::vector<RINTERVAL> &weinerVec, uint64_t weinerVecSize, uint64_t rank, RIntervalStorage<INDEX_SIZE> &outputSet)
            {
                intervalTemporary.clear();
                uint64_t i = rank;

                INDEX_SIZE lcpIntvBeginPos = this->_RLBWTDS->get_fpos(lcpIntv.beginIndex, lcpIntv.beginDiff);
                INDEX_SIZE lcpIntvEndPos = this->_RLBWTDS->get_fpos(lcpIntv.endIndex, lcpIntv.endDiff);

                while (i < weinerVecSize)
                {
                    INDEX_SIZE weinerBeginPos = this->_RLBWTDS->get_fpos(weinerVec[i].beginIndex, weinerVec[i].beginDiff);
                    INDEX_SIZE weinerEndPos = this->_RLBWTDS->get_fpos(weinerVec[i].endIndex, weinerVec[i].endDiff);
                    if (lcpIntvBeginPos <= weinerBeginPos && weinerEndPos <= lcpIntvEndPos)
                    {
                        this->computeNextIntervals(weinerVec[i], true, this->intervalTemporary);
                    }
                    else
                    {
                        break;
                    }

                    i++;
                }
                this->computeNextIntervals(lcpIntv, false, this->intervalTemporary);

                intervalTemporary.move(outputSet);
            }

        public:
            void clear(){
                for(uint64_t i=0;i<this->checkerArray.size();i++){
                    this->checkerArray[i] = false;
                }
                this->intervalTemporary.clear();
            }
            void computeFirstLCPIntervalSet(RIntervalStorage<INDEX_SIZE> &output)
            {
                output.clear();
                RINTERVAL lcpIntv;
                size_t minIndex = this->_RLBWTDS->_rlbwt.get_end_rle_lposition();

                uint64_t maxIndex = 0;
                UCHAR c = 0;
                for (uint64_t i = 0; i < this->_RLBWTDS->_rlbwt.rle_size(); i++)
                {
                    UCHAR c2 = this->_RLBWTDS->_rlbwt.get_char_by_run_index(i);
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
                uint64_t diff = this->_RLBWTDS->_rlbwt.get_run(maxIndex) - 1;
                lcpIntv.beginIndex = minIndex;
                lcpIntv.beginDiff = 0;
                lcpIntv.endIndex = maxIndex;
                lcpIntv.endDiff = diff;

                uint64_t begin_lindex = 0;
                uint64_t begin_diff = 0;
                uint64_t end_lindex = this->_RLBWTDS->_rlbwt.rle_size() - 1;
                uint64_t end_diff = this->_RLBWTDS->_rlbwt.get_run(end_lindex) - 1;

                RINTERVAL tmpArg;
                tmpArg.beginIndex = begin_lindex;
                tmpArg.beginDiff = begin_diff;
                tmpArg.endIndex = end_lindex;
                tmpArg.endDiff = end_diff;
                //std::vector<CHAR> charOutputVec;
                uint64_t resultCount = this->_RLBWTDS->rangeOnRLBWT.range_distinct(tmpArg);
                assert(resultCount > 0);

                uint64_t counter = 0;
                for (uint64_t x = 0; x < resultCount; x++)
                {
                    auto &it = this->_RLBWTDS->rangeOnRLBWT.rIntervalTmpVec[x];
                    if (this->checkWeinerInterval(it))
                    {
                        output.push_weiner(it);

                        counter++;
                    }
                }

                output.push(lcpIntv, counter);
            }
            void computeNextLCPIntervalSet(RIntervalStorage<INDEX_SIZE> &inputSet, RIntervalStorage<INDEX_SIZE> &output)
            {
                output.clear();
                uint64_t rank = 0;
                for (uint64_t i = 0; i < inputSet.lcpIntvCount; i++)
                {
                    this->computeNextLCPIntervalSet(inputSet.lcpIntvVec[i], inputSet.weinerVec, inputSet.weinerCount, rank, output);
                    rank += inputSet.widthVec[i];
                }
            }
        };
    } // namespace lcp_on_rlbwt
} // namespace stool