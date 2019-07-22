#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "mycode.hpp"
#include <stack>
#include <queue>

namespace stool
{
/*
template <typename INDEXTYPE>
class LCPIntervalIterator
{
    vector<INDEXTYPE> *SA;       // left boundaries of internal node
    vector<INDEXTYPE> *LCPArray; // right boundaries of internal node
    std::stack<LCPInterval<INDEXTYPE>> stack;
    std::queue<LCPInterval<INDEXTYPE>> outputQueue;

    INDEXTYPE current_x = 0;
    uint64_t _current_i = 0;

public:
    LCPIntervalIterator() = default;

    LCPIntervalIterator(vector<INDEXTYPE> *_SA, vector<INDEXTYPE> *_LCPArray) : SA(_SA), LCPArray(_LCPArray)
    {

    }

    void createLCPIntervals()
    {
        //stack<LCPInterval> stack;
        int64_t n = sa.size();

        if (current_x < n)
        {
            LCPInterval fst = LCPInterval(i, i, n - sa[i]);
            while (stack.size() > 1)
            {
                LCPInterval second = stack.top();
                if (lcp[second.i] > lcp[fst.i])
                {
                    stack.pop();
                    uint64_t newLCP = lcp[second.i];
                    uint64_t newj = second.j;
                    uint64_t newi = second.i;

                    while (stack.size() > 0)
                    {
                        LCPInterval third = stack.top();
                        newi = third.i;
                        stack.pop();
                        if (lcp[second.i] != lcp[third.i])
                        {
                            break;
                        }
                    }
                    LCPInterval newInterval(newi, newj, newLCP);
                    stack.push(newInterval);
                    outputQueue.push(newInterval);
                    //intervals.push_back(newInterval);
                }
                else
                {
                    break;
                }
            }
            //intervals.push_back(fst);
            outputQueue.push(fst);
            stack.push(fst);
            current_x++;
        }
        else if (current_x == n)
        {
            while (stack.size() > 1)
            {
                LCPInterval second = stack.top();
                stack.pop();
                uint64_t newLCP = lcp[second.i];
                uint64_t newj = second.j;
                uint64_t newi = second.i;
                while (stack.size() > 0)
                {
                    LCPInterval third = stack.top();
                    newi = third.i;
                    stack.pop();
                    if (lcp[second.i] != lcp[third.i])
                    {
                        break;
                    }
                }
                LCPInterval newInterval(newi, newj, newLCP);
                stack.push(newInterval);
                //intervals.push_back(newInterval);
                outputQueue.push(newInterval);
            }
            current_x = UINT64_MAX;
        }else{

        }
    }

    LCPIntervalIterator &operator++()
    {
        outputQueue.pop();
        while(outputQueue.size() == 0 && current_x != UINT64_MAX){
            this->createLCPIntervals();
        }

        if(outputQueue.size() > 0){
            this->_current_i++;
        }else{
            this->_current_i = UINT64_MAX;

        }

        return *this;
    }
    stool::LCPInterval<INDEXTYPE> operator*()
    {
        LCPInterval<INDEXTYPE> p = outputQueue.top();
        return p;
        //return this->_current_interval;
    }
    bool operator!=(const LCPIntervalIterator &rhs)
    {
        //std::cout << _current_i << "/"
        return _current_i != rhs._current_i;
    }
    bool isEnd()
    {
        return _current_i == UINT64_MAX;
    }
};

template <typename INDEXTYPE>
class PostorderLCPIntervals
{
public:
    //vector<char> &T;
    vector<INDEXTYPE> &SA; // suffix array
    vector<INDEXTYPE> &LCPArray;
    //vector<INDEXTYPE> R;  // right boundaries of internal node
    //vector<INDEXTYPE> D;  // depths of internal node
    INDEXTYPE nodeNum;
    INDEXTYPE SA_first_index = UINT64_MAX;
    INDEXTYPE _size;
    LCPIntervalIterator<INDEXTYPE> *_end;

public:
    ~PostorderLCPIntervals()
    {
        delete this->_end;
    }

    PostorderLCPIntervals(vector<char> &T)
    {
        this->_size = T.size();
        this->SA.resize(_size);
        this->L.resize(_size);
        this->R.resize(_size);
        this->D.resize(_size);

        INDEXTYPE alphaSize = 0x100; // This can be very large

        // Computing internal nodes of the suffix tree of the input file.
        if (esaxx(T.begin(), SA.begin(),
                  L.begin(), R.begin(), D.begin(),
                  this->_size, alphaSize, this->nodeNum) == -1)
        {
            throw - 1;
        }

        for (INDEXTYPE i = 0; i < this->_size; i++)
        {
            std::cout << i << ":" << SA[i] << std::endl;
            if (SA[i] == 0)
                SA_first_index = i;
        }

        this->_end = new LCPIntervalIterator<INDEXTYPE>(this->L, this->R, this->D, this->nodeNum, false);
    }
    LCPIntervalIterator<INDEXTYPE> begin()
    {
        return LCPIntervalIterator<INDEXTYPE>(this->L, this->R, this->D, this->nodeNum, true);
    }
    LCPIntervalIterator<INDEXTYPE> end()
    {
        return *this->_end;
    }
};
*/
} // namespace stool
