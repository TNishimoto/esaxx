#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>

namespace stool
{
    namespace lcp_on_rlbwt
    {
        template <typename INDEX_SIZE>
        struct WeinerInterval
        {
            INDEX_SIZE beginIndex;
            INDEX_SIZE beginDiff;
            INDEX_SIZE endIndex;
            INDEX_SIZE endDiff;

            void print()
            {
                std::cout << "[" << this->beginIndex << ", " << this->beginDiff << ", " << this->endIndex << ", " << this->endDiff << "]" << std::endl;
            }
            void print2(std::vector<INDEX_SIZE> &fposArray)
            {
                if (this->is_special())
                {
                    std::cout << "[BOTTOM]" << std::endl;
                }
                else
                {
                    INDEX_SIZE begin_pos = fposArray[this->beginIndex] + this->beginDiff;
                    INDEX_SIZE end_pos = fposArray[this->endIndex] + this->endDiff;

                    std::cout << "[" << begin_pos << ", " << end_pos << "]" << std::endl;
                }
            }

            bool is_special()
            {
                return this->beginIndex == std::numeric_limits<INDEX_SIZE>::max();
            }

            static WeinerInterval get_special()
            {
                WeinerInterval r;
                r.beginIndex = std::numeric_limits<INDEX_SIZE>::max();
                return r;
            }
        };
    }
}