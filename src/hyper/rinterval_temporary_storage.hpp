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
//#include "../../module/rlbwt_iterator/src/include/weiner/sampling_functions.hpp"
//#include "range_distinct/range_distinct_on_rlbwt.hpp"

namespace stool
{
    namespace lcp_on_rlbwt
    {

        template <typename INDEX_SIZE>
        class RIntervalTemporaryStorage
        {
            using RINTERVAL = RInterval<INDEX_SIZE>;

            std::vector<std::vector<RINTERVAL>> weinerIntervalVec;
            std::vector<RINTERVAL> lcpIntervalVec;

            std::vector<uint64_t> indexVec;
            std::vector<bool> weinerIntervalOccurrenceBitArray;
            std::vector<bool> lcpIntvOccurrenceBitArray;

            std::vector<uint64_t> countVec;
            uint64_t indexCount = 0;

        public:
            void initialize()
            {
                uint64_t CHARMAX = UINT8_MAX + 1;
                weinerIntervalVec.resize(CHARMAX);
                for (uint64_t i = 0; i < CHARMAX; i++)
                {
                    weinerIntervalVec[i].resize(2);
                }
                indexVec.resize(CHARMAX);
                weinerIntervalOccurrenceBitArray.resize(CHARMAX, false);
                countVec.resize(CHARMAX, 0);

                lcpIntvOccurrenceBitArray.resize(CHARMAX, false);
                lcpIntervalVec.resize(CHARMAX);
            }
            bool occur(uint64_t character)
            {
                return this->weinerIntervalOccurrenceBitArray[character];
            }

            void clear()
            {
                for (uint64_t i = 0; i < this->indexCount; i++)
                {
                    auto &it = this->indexVec[i];
                    weinerIntervalOccurrenceBitArray[it] = false;
                    countVec[it] = 0;

                    lcpIntvOccurrenceBitArray[it] = false;
                }
                indexCount = 0;
            }

            void move(RIntervalStorage<INDEX_SIZE> &output)
            {
                for (uint64_t i = 0; i < this->indexCount; i++)
                {
                    auto character = this->indexVec[i];
                    auto &currentVec = this->weinerIntervalVec[character];
                    uint64_t count = this->countVec[character];
                    if (this->weinerIntervalOccurrenceBitArray[character])
                    {
                        for (uint64_t j = 0; j < count; j++)
                        {
                            output.push_weiner(currentVec[j]);
                        }
                        output.push(this->lcpIntervalVec[character], count);
                    }
                }
            }
            void pushWeinerInterval(RINTERVAL &w, uint8_t c)
            {
                if (!this->weinerIntervalOccurrenceBitArray[c])
                {
                    this->indexVec[this->indexCount] = c;
                    this->indexCount++;
                    this->weinerIntervalOccurrenceBitArray[c] = true;
                }
                if (this->countVec[c] == this->weinerIntervalVec[c].size())
                {
                    this->weinerIntervalVec[c].resize(this->countVec[c] * 2);
                }
                this->weinerIntervalVec[c][this->countVec[c]] = w;
                this->countVec[c]++;
            }
            void pushLCPInterval(RINTERVAL &w, uint8_t c)
            {
                
                this->lcpIntervalVec[c] = w;
                this->lcpIntvOccurrenceBitArray[c] = true;
            }
            bool checkWeinerInterval(RINTERVAL &w, uint8_t c)
            {
                auto& lcpIntv = this->lcpIntervalVec[c];
                //bool b1 = lcpIntv.beginIndex == w.beginIndex && lcpIntv.beginDiff == w.beginDiff;
                bool b2 = lcpIntv.endIndex == w.endIndex && lcpIntv.endDiff == w.endDiff;

                //std::cout << c << "/" << b1 << "/" << b2 << std::endl;
                return !b2;
            }
        };

    } // namespace lcp_on_rlbwt
} // namespace stool