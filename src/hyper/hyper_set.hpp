#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <vector>
#include <type_traits>
#include "../../module/rlbwt_iterator/src/include/sampling_functions.hpp"
#include "./range_distinct_on_rlbwt.hpp"
#include "./succinct_range_distinct.hpp"

namespace stool
{
    namespace lcp_on_rlbwt
    {

        template <typename INDEX_SIZE>
        class HyperSet
        {
        public:
            using WEINER = WeinerInterval<INDEX_SIZE>;
            std::vector<WEINER> lcpIntvVec;
            std::vector<WEINER> weinerVec;
            std::vector<uint8_t> widthVec;

            uint64_t lcpIntvCount = 0;
            uint64_t weinerCount = 0;

            HyperSet()
            {
                this->lcpIntvVec.resize(8);
                this->weinerVec.resize(8);
                this->widthVec.resize(8);
            }

            void swap(HyperSet &copy)
            {
                this->lcpIntvVec.swap(copy.lcpIntvVec);
                this->weinerVec.swap(copy.weinerVec);
                this->widthVec.swap(copy.widthVec);

                uint64_t tmp1 = this->lcpIntvCount;
                this->lcpIntvCount = copy.lcpIntvCount;
                copy.lcpIntvCount = tmp1;

                uint64_t tmp2 = this->weinerCount;
                this->weinerCount = copy.weinerCount;
                copy.weinerCount = tmp2;
            }
            void push(WEINER &w, uint64_t width)
            {

                if (this->lcpIntvCount == this->lcpIntvVec.size())
                {
                    this->lcpIntvVec.resize(this->lcpIntvCount * 2);
                    this->widthVec.resize(this->lcpIntvCount * 2);
                }
                this->lcpIntvVec[this->lcpIntvCount] = w;
                this->widthVec[this->lcpIntvCount] = width;
                this->lcpIntvCount++;
            }
            void push_weiner(WEINER &w)
            {

                if (this->weinerCount == this->weinerVec.size())
                {
                    this->weinerVec.resize(this->weinerCount * 2);
                }
                this->weinerVec[this->weinerCount] = w;
                this->weinerCount++;
            }
            void clear()
            {
                this->lcpIntvCount = 0;
                this->weinerCount = 0;
            }
        };

        template <typename INDEX_SIZE>
        class IntervalTemporary
        {
            using WEINER = WeinerInterval<INDEX_SIZE>;

            std::vector<std::vector<WEINER>> intervalVec;
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

            void move(HyperSet<INDEX_SIZE> &item)
            {
                for (uint64_t i = 0; i < this->indexCount; i++)
                {
                    auto &it = this->indexVec[i];
                    auto &currentVec = this->intervalVec[it];
                    uint64_t count = this->countVec[it];
                    if (count > 1)
                    {
                        item.push(currentVec[0], count - 1);
                        for (uint64_t j = 1; j < count; j++)
                        {
                            item.push_weiner(currentVec[j]);
                        }
                    }
                }
            }
            void push(WeinerInterval<INDEX_SIZE> &w, uint8_t c)
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

    } // namespace lcp_on_rlbwt
} // namespace stool