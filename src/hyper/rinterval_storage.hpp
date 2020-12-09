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
        class RIntervalStorage
        {
            using RINTERVAL = RInterval<INDEX_SIZE>;

        public:
            std::vector<RINTERVAL> lcpIntvVec;
            std::vector<RINTERVAL> weinerVec;
            std::vector<uint8_t> widthVec;

            //RLBWTDS *_RLBWTDS;
            uint64_t lcpIntvCount = 0;
            uint64_t weinerCount = 0;

            RIntervalStorage()
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

            void swap(RIntervalStorage &copy)
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
            void computeNextLCPIntervalSet(RIntervalStorage<INDEX_SIZE, RLBWTDS> &inputSet, ExplicitWeinerLinkEmulator<INDEX_SIZE, RLBWTDS> &em)
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

                assert(this->lcpIntvCount > 0);
            }

        private:
            /*
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
            */
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