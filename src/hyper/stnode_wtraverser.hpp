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
#include "weiner_link_emulator.hpp"
//#include "range_distinct/range_distinct_on_rlbwt.hpp"

namespace stool
{
    namespace lcp_on_rlbwt
    {
        template <typename INDEX_SIZE, typename RLBWTDS>
        class STNodeWTraverser
        {
            using RINTERVAL = RInterval<INDEX_SIZE>;

        public:
            std::vector<RINTERVAL> lcpIntvVec;
            std::vector<RINTERVAL> weinerVec;
            std::vector<uint8_t> widthVec;

            //RLBWTDS *_RLBWTDS;
            uint64_t lcpIntvCount = 0;
            uint64_t weinerCount = 0;

            STNodeWTraverser()
            {
                this->lcpIntvVec.resize(8);
                this->weinerVec.resize(8);
                this->widthVec.resize(8);
                //this->_RLBWTDS = _rlbwtds;
            }

            const std::vector<RINTERVAL> *getLcpIntvVec() const
            {
                return &this->lcpIntvVec;
            }

            const std::vector<RINTERVAL> *getWeinerVec() const
            {
                return &this->weinerVec;
            }
            uint64_t size(){
                return this->lcpIntvCount;
            }

            void swap(STNodeWTraverser &copy)
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
            void clear()
            {
                this->lcpIntvCount = 0;
                this->weinerCount = 0;
            }
            void first_compute(ExplicitWeinerLinkEmulator<INDEX_SIZE, RLBWTDS> &em)
            {
                em.computeFirstLCPIntervalSet();
                this->move_from(em);
            }
            void computeNextLCPIntervalSet(STNodeWTraverser<INDEX_SIZE, RLBWTDS> &inputSet, ExplicitWeinerLinkEmulator<INDEX_SIZE, RLBWTDS> &em)
            {
                assert(inputSet.lcpIntvCount > 0);

                this->clear();
                uint64_t rank = 0;
                //std::cout << "a" << inputSet.lcpIntvCount << std::endl;
                for (uint64_t i = 0; i < inputSet.lcpIntvCount; i++)
                {
                    em.computeNextLCPIntervalSet(inputSet.lcpIntvVec[i], inputSet.weinerVec, inputSet.weinerCount, rank);
                    this->move_from(em);
                    rank += inputSet.widthVec[i];
                }

                //assert(this->lcpIntvCount > 0);
            }
            void spill(STNodeWTraverser<INDEX_SIZE, RLBWTDS> &item, uint64_t limit_child_count){
                //std::cout << "SPILL" << limit_child_count << std::endl;
                //this->print();
                //item.print();
                assert(item.weinerCount <= limit_child_count);

                uint64_t capacity = limit_child_count - item.weinerCount;
                uint64_t k = 0;
                uint64_t k2 = 0;
                int64_t x = this->lcpIntvCount-1;
                while(x >= 0 && k <= capacity){
                    k += this->widthVec[x--];
                    k2++;
                }
                uint64_t newChildrenCount = item.weinerCount + k;
                if(newChildrenCount >= item.weinerVec.size()){
                    item.weinerVec.resize(newChildrenCount * 2);
                }
                for(uint64_t i = this->weinerCount - k;i < this->weinerCount;i++){
                    item.weinerVec[item.weinerCount] = this->weinerVec[i];
                    item.weinerCount++;
                }
                this->weinerCount -= k;

                uint64_t newSTNodeCount = item.lcpIntvCount + k2;
                if(newSTNodeCount >= item.lcpIntvVec.size()){
                    item.lcpIntvVec.resize(newSTNodeCount * 2);
                    item.widthVec.resize(newSTNodeCount * 2);
                }

                for(uint64_t i = this->lcpIntvCount - k2;i < this->lcpIntvCount;i++){
                    item.lcpIntvVec[item.lcpIntvCount] = this->lcpIntvVec[i];
                    item.widthVec[item.lcpIntvCount] = this->widthVec[i];
                    item.lcpIntvCount++;
                }
                this->lcpIntvCount -= k2;

                //std::cout << "SPILL EMD" << std::endl;
                //this->print();
                //item.print();

            }
            void print(){
                std::cout << "[" << this->lcpIntvCount << ", " << this->weinerCount << "]" << std::endl;
                
            }

        private:
            void move_from(ExplicitWeinerLinkEmulator<INDEX_SIZE, RLBWTDS> &em)
            {
                uint64_t stnodeCount = em.get_explicit_stnode_count();
                uint64_t newSTNodeCount = this->lcpIntvCount + stnodeCount;
                if (newSTNodeCount >= this->lcpIntvVec.size())
                {
                    this->lcpIntvVec.resize(newSTNodeCount * 2);
                    this->widthVec.resize(newSTNodeCount * 2);
                }

                uint64_t childrenCount = em.get_explicit_children_count();
                uint64_t newChildrenCount = this->weinerCount + childrenCount;
                if (newChildrenCount >= this->weinerVec.size())
                {
                    this->weinerVec.resize(newChildrenCount * 2);
                }

                em.move_st_internal_nodes(this->lcpIntvVec, this->weinerVec, this->widthVec, this->lcpIntvCount, this->weinerCount);
                this->lcpIntvCount = newSTNodeCount;
                this->weinerCount = newChildrenCount;
            }
        };

    } // namespace lcp_on_rlbwt
} // namespace stool