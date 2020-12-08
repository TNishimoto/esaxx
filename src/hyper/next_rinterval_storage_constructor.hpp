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

        template <typename INDEX_SIZE, typename RLBWTDS>
        class NextRIntervalStorageConstructor
        {

            using CHAR = typename RLBWTDS::CHAR;

            using RINTERVAL = RInterval<INDEX_SIZE>;
            using UCHAR = typename std::make_unsigned<CHAR>::type;

            //std::vector<bool> checkerArray;
            RLBWTDS *_RLBWTDS;
            RIntervalTemporaryStorage<INDEX_SIZE> intervalTemporary;

            /*
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
            */

        public:
            void initialize(RLBWTDS *_rlbwtds)
            {
                this->_RLBWTDS = _rlbwtds;
                //uint64_t runSize = _rlbwtds->rle_size();
                //uint64_t CHARMAX = UINT8_MAX + 1;
                //this->checkerArray.resize(runSize, false);
                intervalTemporary.initialize();
            }

        private:
            void getNextSAIntervalsForLCPIntervals(const RINTERVAL &w, RIntervalTemporaryStorage<INDEX_SIZE> &output)
            {
                RINTERVAL frontL = this->_RLBWTDS->getIntervalOnL(w);
                uint64_t resultCount = this->_RLBWTDS->rangeOnRLBWT.range_distinct(frontL);
                for (uint64_t i = 0; i < resultCount; i++)
                {
                    UCHAR c = this->_RLBWTDS->rangeOnRLBWT.charTmpVec[i];
                    auto &it = this->_RLBWTDS->rangeOnRLBWT.rIntervalTmpVec[i];

                    output.pushLCPInterval(it, c);
                }
            }

            void computeNextIntervals(const RINTERVAL &w, RIntervalTemporaryStorage<INDEX_SIZE> &output)
            {
                RINTERVAL frontL = this->_RLBWTDS->getIntervalOnL(w);

                assert(frontL.beginIndex <= frontL.endIndex);


                uint64_t resultCount = this->_RLBWTDS->rangeOnRLBWT.range_distinct(frontL);

                for (uint64_t i = 0; i < resultCount; i++)
                {
                    UCHAR c = this->_RLBWTDS->rangeOnRLBWT.charTmpVec[i];
                    auto &it = this->_RLBWTDS->rangeOnRLBWT.rIntervalTmpVec[i];
                    if (output.checkWeinerInterval(it, c))
                    {
                        output.pushWeinerInterval(it, c);
                    }
                    /*
                    if (isWeiner)
                    {
                    }
                    else
                    {
                        if (output.occur(c))
                        {
                            it.print2(_RLBWTDS->_fposDS);
                            output.pushLCPInterval(it, c);
                        }
                    }
                    */

                }
            }
            void computeNextLCPIntervalSet(const RINTERVAL &lcpIntv, const std::vector<RINTERVAL> &weinerVec, uint64_t weinerVecSize, uint64_t rank, RIntervalStorage<INDEX_SIZE> &outputSet)
            {

                
                //std::cout << "Next"  << std::endl;
                //lcpIntv.print2(_RLBWTDS->_fposDS);
                

                intervalTemporary.clear();
                uint64_t i = rank;

                INDEX_SIZE lcpIntvBeginPos = this->_RLBWTDS->get_fpos(lcpIntv.beginIndex, lcpIntv.beginDiff);
                INDEX_SIZE lcpIntvEndPos = this->_RLBWTDS->get_fpos(lcpIntv.endIndex, lcpIntv.endDiff);

                this->getNextSAIntervalsForLCPIntervals(lcpIntv, this->intervalTemporary);

                while (i < weinerVecSize)
                {

                    INDEX_SIZE weinerBeginPos = this->_RLBWTDS->get_fpos(weinerVec[i].beginIndex, weinerVec[i].beginDiff);
                    INDEX_SIZE weinerEndPos = this->_RLBWTDS->get_fpos(weinerVec[i].endIndex, weinerVec[i].endDiff);
                    if (lcpIntvBeginPos <= weinerBeginPos && weinerEndPos <= lcpIntvEndPos)
                    {
                        this->computeNextIntervals(weinerVec[i], this->intervalTemporary);
                    }
                    else
                    {

                        break;
                    }

                    i++;
                }

                intervalTemporary.move(outputSet);
            }

        public:
            void clear()
            {
                this->intervalTemporary.clear();
            }
            void computeFirstLCPIntervalSet(RIntervalStorage<INDEX_SIZE> &output)
            {
                output.clear();
                RINTERVAL lcpIntv;
                size_t minIndex = this->_RLBWTDS->get_end_rle_lposition();

                uint64_t maxIndex = 0;
                UCHAR c = 0;
                for (uint64_t i = 0; i < this->_RLBWTDS->rle_size(); i++)
                {
                    UCHAR c2 = this->_RLBWTDS->get_char_by_run_index(i);
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
                uint64_t diff = this->_RLBWTDS->get_run(maxIndex) - 1;
                lcpIntv.beginIndex = minIndex;
                lcpIntv.beginDiff = 0;
                lcpIntv.endIndex = maxIndex;
                lcpIntv.endDiff = diff;

                uint64_t begin_lindex = 0;
                uint64_t begin_diff = 0;
                uint64_t end_lindex = this->_RLBWTDS->rle_size() - 1;
                uint64_t end_diff = this->_RLBWTDS->get_run(end_lindex) - 1;

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
                    assert(this->_RLBWTDS->get_char_by_run_index(it.beginIndex) == this->_RLBWTDS->get_char_by_run_index(it.endIndex));
                    output.push_weiner(it);
                    

                    counter++;

                    //if (this->checkWeinerInterval(it))
                    //{
                    //}
                }

                output.push(lcpIntv, counter);
            }
            void computeNextLCPIntervalSet(RIntervalStorage<INDEX_SIZE> &inputSet, RIntervalStorage<INDEX_SIZE> &output)
            {
                assert(inputSet.lcpIntvCount > 0);

                output.clear();
                uint64_t rank = 0;
                //std::cout << "a" << inputSet.lcpIntvCount << std::endl;
                for (uint64_t i = 0; i < inputSet.lcpIntvCount; i++)
                {
                    this->computeNextLCPIntervalSet(inputSet.lcpIntvVec[i], inputSet.weinerVec, inputSet.weinerCount, rank, output);

                    rank += inputSet.widthVec[i];
                }
                /*
                std::cout << "ENUMERATE" << std::endl;
                for (uint64_t i = 0; i < output.lcpIntvCount; i++)
                {
                    output.lcpIntvVec[i].print2(this->_RLBWTDS->fposArray);
                }
                */

                assert(output.lcpIntvCount > 0);
            }
        };
    } // namespace lcp_on_rlbwt
} // namespace stool