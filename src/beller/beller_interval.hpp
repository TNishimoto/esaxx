#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>

#include "char_interval.hpp"

//#include "sa_lcp.hpp"
using namespace std;
using namespace sdsl;

namespace stool
{
    namespace beller
    {
        std::vector<stool::LCPInterval<uint64_t>> computeLCPIntervals(int_vector<> &bwt, std::vector<uint64_t> &C, sdsl::wt_huff<> &wt)
        {

            using INTERVAL = stool::LCPInterval<uint64_t>;
            uint64_t n = bwt.size();
            std::vector<std::queue<INTERVAL>> queArr;
            std::vector<uint64_t> checker;
            std::vector<uint64_t> counter;
            queArr.resize(UINT8_MAX);
            counter.resize(UINT8_MAX, 0);
            checker.resize(bwt.size() + 1, false);
            uint8_t lastChar = bwt[bwt.size() - 1];
            uint64_t max_interval_count = 0;

            std::vector<INTERVAL> r;

            std::cout << "bwt: " << n << std::endl;
            checker[0] = 0;

            INTERVAL fst(0, n - 1, 0);
            auto tmp1 = CharInterval::getIntervals(fst.i, fst.j, C, wt, lastChar);

            for (auto intv : tmp1)
            {
                queArr[intv.c].push(INTERVAL(intv.i, intv.j, 1));
            }

            uint64_t lcp = 0;
            while (true)
            {
                uint64_t interval_count = 0;
                uint64_t last_idx = UINT64_MAX;
                uint64_t last_lb = UINT64_MAX;

                bool occB = false;
                lcp++;
                for (uint64_t x = 0; x < queArr.size(); x++)
                {
                    auto &que = queArr[x];
                    uint64_t queSize = que.size();
                    counter[x] = queSize;
                    interval_count += queSize;
                }
                if(max_interval_count < interval_count) max_interval_count = interval_count;
                std::cout << "lcp = " << lcp << ", count = " << interval_count << ", max = " << max_interval_count << std::endl;

                for (uint64_t x = 0; x < queArr.size(); x++)
                {
                    auto &que = queArr[x];
                    uint64_t queSize = counter[x];

                    while (queSize > 0)
                    {
                        occB = true;

                        auto top = que.front();
                        que.pop();
                        queSize--;

                        //std::cout << top.to_string() << ",checker[" << (top.j + 1) << "]=" << checker[top.j + 1] << ", last_idx = " << last_idx << std::endl;

                        if (checker[top.j + 1] == 0)
                        {
                            if (last_lb == UINT64_MAX)
                            {
                                last_lb = top.i;
                            }

                            checker[top.j + 1] += 1;
                            last_idx = top.j + 1;
                            //auto tmp = getIntervals(top.i, top.j, bwt, C, wt);

                            auto tmpx = CharInterval::getIntervals(top.i, top.j, C, wt, lastChar);
                            //check(tmp, tmpx);

                            for (auto intv : tmpx)
                            {                                
                                queArr[intv.c].push(INTERVAL(intv.i, intv.j, top.lcp + 1));
                            }
                        }
                        else
                        {
                            checker[top.j + 1] += 1;
                            if (top.i == last_idx)
                            {
                                r.push_back(INTERVAL(last_lb, top.j, top.lcp - 1));

                                last_lb = UINT64_MAX;
                                last_idx = UINT64_MAX;

                                //auto tmp = getIntervals(top.i, top.j, bwt, C, wt);
                                auto tmpx = CharInterval::getIntervals(top.i, top.j, C, wt, lastChar);

                                //check(tmp, tmpx);

                                for (auto intv : tmpx)
                                {
                                    queArr[intv.c].push(INTERVAL(intv.i, intv.j, top.lcp + 1));
                                }
                            }
                        }
                    }
                }
                if (!occB)
                    break;
            }

            checker.pop_back();

            return r;
        }
    } // namespace LCPIntervals

} // namespace stool