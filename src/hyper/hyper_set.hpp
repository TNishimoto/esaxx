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
//#include "range_distinct/range_distinct_on_rlbwt.hpp"

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
            void convertFrom(){
                
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

        

    } // namespace lcp_on_rlbwt
} // namespace stool