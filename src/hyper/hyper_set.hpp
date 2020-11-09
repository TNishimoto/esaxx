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
#include "range_distinct/range_distinct_on_rlbwt.hpp"

namespace stool
{
    namespace lcp_on_rlbwt
    {

        template <typename INDEX_SIZE>
        class HyperSet
        {
        public:
            using RINTERVAL = RInterval<INDEX_SIZE>;
            std::vector<RINTERVAL> lcpIntvVec;
            std::vector<RINTERVAL> weinerVec;
            std::vector<uint8_t> widthVec;

            uint64_t lcpIntvCount = 0;
            uint64_t weinerCount = 0;

            HyperSet()
            {
                this->lcpIntvVec.resize(8);
                this->weinerVec.resize(8);
                this->widthVec.resize(8);
            }

            const std::vector<RINTERVAL> *getLcpIntvVec() const {
                return &this->lcpIntvVec;
            }


            const std::vector<RINTERVAL> *getWeinerVec() const {
                return &this->weinerVec;
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
            void push(RINTERVAL &w, uint64_t width)
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
            void push_weiner(RINTERVAL &w)
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
        template <typename INDEX_SIZE, typename CHAR>
        class WeinerDataStructures
        {

        public:
            using RINTERVAL = RInterval<INDEX_SIZE>;
            std::vector<bool> checkerArray;
            std::vector<CHAR> charTmpVec;
            vector<RINTERVAL> weinerTmpVec;

            void initialize(uint64_t runSize)
            {
                uint64_t CHARMAX = UINT8_MAX + 1;
                this->checkerArray.resize(runSize, false);

                charTmpVec.resize(CHARMAX);
                weinerTmpVec.resize(CHARMAX);
            }
            
            template <typename RLBWT_STR>
            bool checkWeinerInterval(RINTERVAL &w, RLBWT_STR &_rlbwt)
            {
                bool b = _rlbwt.get_run(w.endIndex) == (w.endDiff + 1);
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
        };

    } // namespace lcp_on_rlbwt
} // namespace stool