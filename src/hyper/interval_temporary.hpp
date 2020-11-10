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
        class IntervalTemporary
        {
            using RINTERVAL = RInterval<INDEX_SIZE>;

            std::vector<std::vector<RINTERVAL>> intervalVec;
            std::vector<uint64_t> indexVec;
            std::vector<bool> bitArray;
            std::vector<uint64_t> countVec;
            uint64_t indexCount = 0;

        public:
            void initialize()
            {
                uint64_t CHARMAX = UINT8_MAX + 1;
                intervalVec.resize(CHARMAX);
                for (uint64_t i = 0; i < CHARMAX; i++)
                {
                    intervalVec[i].resize(2);
                }
                indexVec.resize(CHARMAX);
                bitArray.resize(CHARMAX, false);
                countVec.resize(CHARMAX, 0);
            }
            bool occur(uint64_t character){
                return this->bitArray[character];
            }

            void clearWeinerTmpVec()
            {
                for (uint64_t i = 0; i < this->indexCount; i++)
                {
                    auto &it = this->indexVec[i];
                    bitArray[it] = false;
                    countVec[it] = 0;
                }
                indexCount = 0;
            }

            void move(HyperSet<INDEX_SIZE> &output)
            {
                for (uint64_t i = 0; i < this->indexCount; i++)
                {
                    auto character = this->indexVec[i];
                    auto &currentVec = this->intervalVec[character];
                    uint64_t count = this->countVec[character];
                    if (count > 1)
                    {
                        for (uint64_t j = 0; j < count-1; j++)
                        {
                            output.push_weiner(currentVec[j]);
                        }

                        output.push(currentVec[count-1], count - 1);
                    }
                }
            }
            void push(RINTERVAL &w, uint8_t c)
            {
                if (!this->bitArray[c])
                {
                    this->indexVec[this->indexCount] = c;
                    this->indexCount++;
                    this->bitArray[c] = true;
                }
                if (this->countVec[c] == this->intervalVec[c].size())
                {
                    this->intervalVec[c].resize(this->countVec[c] * 2);
                }
                this->intervalVec[c][this->countVec[c]] = w;
                this->countVec[c]++;
            }
        };


    }
}