#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "mycode.hpp"
#include "lcp_interval_iterator.hpp"

/*
template <typename INDEXTYPE>
class MSIntervalIterator
{
    LCPIntervalIterator<INDEXTYPE> &lcp_iterator;
    vector<INDEXTYPE> &rank;
    //stool::LCPInterval<INDEXTYPE> _current_interval;

    MSIntervalIterator &operator++()
    {
        while (!lcp_iterator.isEnd())
        {
            ++lcp_iterator;
            stool::LCPInterval<INDEXTYPE> interval = *lcp_iterator;
            if ((rank[interval.j - 1] - rank[interval.i] == 0))
            {
            }
            else
            {
                break;
            }
        }
        return *this;
    }
    stool::LCPInterval<INDEXTYPE> operator*()
    {
        return this->_current_interval;
    }
    bool operator!=(const MSIntervalIterator &rhs)
    {
        return this->lcp_iterator != rhs.lcp_iterator;
    }
    bool isEnd()
    {
        return this->lcp_iterator.isEnd();
    }
};

template <typename INDEXTYPE>
class MSIntervalIteratorGenerator
{
    LCPIntervalIteratorGenerator<INDEXTYPE> lcp_interval_iterator_generator;
    vector<INDEXTYPE> rank;

    public:
    MSIntervalIteratorGenerator(vector<char> &T)
    {
        
        this->_end = new LCPIntervalIterator<INDEXTYPE>(this->L, this->R, this->D, this->nodeNum, false);
    }
};

template <typename INDEXTYPE>
vector<INDEXTYPE> constructMSVec(vector<INDEXTYPE> &L, vector<INDEXTYPE> &R, vector<INDEXTYPE> &D)
{
    std::unordered_map<INDEXTYPE, INDEXTYPE> mapper;

    INDEXTYPE size = L.size();
    vector<INDEXTYPE> r;
    r.resize(size, std::numeric_limits<INDEXTYPE>::max());

    for (INDEXTYPE i = 0; i < size; i++)
    {
        INDEXTYPE width = R[i] - L[i] + 1;
        INDEXTYPE x = (1 + R[i]) - width;
        while (width > 0)
        {
            auto it = mapper.find(x);
            if (it != mapper.end())
            {
                INDEXTYPE childID = *it;
                INDEXTYPE childWidth = R[childID] - L[childID] + 1;
                r[childID] = i;
                width -= childWidth;
                x = (1 + R[i]) - width;
                mapper.erase(x);
            }
            else
            {
                break;
            }
        }
        mapper[L[i]] = i;
    }
    return r;
}

template <typename INDEXTYPE>
vector<INDEXTYPE> constructParentVec(vector<INDEXTYPE> &L, vector<INDEXTYPE> &R, vector<INDEXTYPE> &D)
{
    std::unordered_map<INDEXTYPE, INDEXTYPE> mapper;

    INDEXTYPE size = L.size();
    vector<INDEXTYPE> r;
    r.resize(size, std::numeric_limits<INDEXTYPE>::max());

    for (INDEXTYPE i = 0; i < size; i++)
    {
        INDEXTYPE width = R[i] - L[i] + 1;
        INDEXTYPE x = (1 + R[i]) - width;
        while (width > 0)
        {
            auto it = mapper.find(x);
            if (it != mapper.end())
            {
                INDEXTYPE childID = *it;
                INDEXTYPE childWidth = R[childID] - L[childID] + 1;
                r[childID] = i;
                width -= childWidth;
                x = (1 + R[i]) - width;
                mapper.erase(x);
            }
            else
            {
                break;
            }
        }
        mapper[L[i]] = i;
    }
    return r;
}
*/