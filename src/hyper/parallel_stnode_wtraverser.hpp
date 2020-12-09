#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <vector>
#include <type_traits>
#include "stnode_wtraverser.hpp"

namespace stool
{
    namespace lcp_on_rlbwt
    {
        template <typename INDEX_SIZE, typename RLBWTDS>
        class ParallelSTNodeWTraverser
        {
            using RINTERVAL = RInterval<INDEX_SIZE>;
            using STNODE_WTRAVERSER = STNodeWTraverser<INDEX_SIZE, RLBWTDS>;

            std::vector<STNODE_WTRAVERSER> sub_trees;
            std::vector<STNODE_WTRAVERSER> sub_tmp_trees;
            std::vector<ExplicitWeinerLinkEmulator<INDEX_SIZE, RLBWTDS>> ems;

        public:
            uint64_t current_lcp = 0;
            uint64_t total_counter = 0;
            uint64_t strSize = 0;
            uint64_t node_count = 0;
            uint64_t child_count = 0;

            uint64_t get_tree_count()
            {
                return this->sub_trees.size();
            }

            void initialize(uint64_t size, RLBWTDS &_RLBWTDS)
            {
                this->strSize = _RLBWTDS.str_size();

                for (uint64_t i = 0; i < size; i++)
                {
                    sub_trees.push_back(STNODE_WTRAVERSER());
                    sub_tmp_trees.push_back(STNODE_WTRAVERSER());
                    ems.push_back(ExplicitWeinerLinkEmulator<INDEX_SIZE, RLBWTDS>());
                    ems[ems.size() - 1].initialize(&_RLBWTDS);
                }
            }
            RINTERVAL &get_stnode(uint64_t index)
            {
                uint64_t x = 0;
                for (uint64_t i = 0; i < this->sub_trees.size(); i++)
                {
                    uint64_t k = this->sub_trees[i].size();
                    if (x <= index && index < x + k)
                    {
                        return this->sub_trees[i].lcpIntvVec[index - x];
                    }
                    else
                    {
                        x += k;
                    }
                }
                std::cout << index << "/" << this->node_count << "/" << x << std::endl;
                assert(false);
                throw -1;
            }
            RINTERVAL &get_child(uint64_t index)
            {
                uint64_t x = 0;
                for (uint64_t i = 0; i < this->sub_trees.size(); i++)
                {
                    uint64_t k = this->sub_trees[i].weinerCount;
                    if (x <= index && index < x + k)
                    {
                        return this->sub_trees[i].weinerVec[index - x];
                    }
                    else
                    {
                        x += k;
                    }
                }
                assert(false);
                throw -1;
            }

            void process(STNODE_WTRAVERSER &tree, STNODE_WTRAVERSER &tmp_tree, ExplicitWeinerLinkEmulator<INDEX_SIZE, RLBWTDS> &em)
            {
                if (tree.lcpIntvCount > 0)
                {
                    tmp_tree.computeNextLCPIntervalSet(tree, em);
                    tree.swap(tmp_tree);
                }
            }
            void process()
            {
                std::cout << "LCP = " << current_lcp << std::endl;
                if (current_lcp > 0)
                {
                    this->allocate_data();
                    for (uint64_t i = 0; i < this->sub_trees.size(); i++)
                    {
                        this->process(sub_trees[i], sub_tmp_trees[i], ems[i]);
                    }
                }
                else
                {
                    this->sub_trees[0].first_compute(ems[0]);
                }
                uint64_t k = 0;
                this->node_count = 0;
                for (uint64_t i = 0; i < this->sub_trees.size(); i++)
                {
                    k += sub_trees[i].weinerCount;
                    this->node_count += sub_trees[i].size();
                }
                total_counter += k;
                this->child_count = k;
                assert(total_counter <= strSize);

                current_lcp++;
                assert(this->child_count > 0);
            }
            bool isStop()
            {
                return total_counter == strSize;
            }
            void allocate_data()
            {
                uint64_t child_avg_count = (this->child_count / this->get_tree_count()) + 256;
                std::queue<uint64_t> lower_indexes, upper_indexes;
                for (uint64_t i = 0; i < this->get_tree_count(); i++)
                {
                    if (this->sub_trees[i].weinerCount <= child_avg_count)
                    {
                        lower_indexes.push(i);
                    }
                    else
                    {
                        upper_indexes.push(i);
                    }
                }
                if(upper_indexes.size() == 0) return;
                std::cout << "ALLOCATE, " << "AVERAGE = " << child_avg_count << std::endl;
                this->print();

                while (upper_indexes.size() > 0)
                {
                    uint64_t i = upper_indexes.front();

                    while (this->sub_trees[i].weinerCount > child_avg_count)
                    {
                        //this->print();

                        //std::cout << "-" << std::endl;
                        //this->sub_trees[i].print();
                        assert(lower_indexes.size() > 0);
                        uint64_t j = lower_indexes.front();
                        //std::cout << i << " -> " << j << std::endl;

                        this->sub_trees[i].spill(this->sub_trees[j], child_avg_count);
                        if (this->sub_trees[j].weinerCount > child_avg_count)
                        {
                            lower_indexes.pop();
                        }
                    }
                    upper_indexes.pop();
                }
                this->print();
            }
            void print()
            {
                for (uint64_t i = 0; i < this->sub_trees.size(); i++)
                {
                    std::cout << "[" << i << ": " << this->sub_trees[i].lcpIntvCount << ", " << this->sub_trees[i].weinerCount << "]" << std::flush;
                }
                std::cout << std::endl;
            }
        };

    } // namespace lcp_on_rlbwt
} // namespace stool