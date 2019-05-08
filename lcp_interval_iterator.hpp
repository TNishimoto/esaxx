#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "mycode.hpp"

namespace stool
{

template <typename INDEXTYPE>
class LCPIntervalIterator
{
    vector<INDEXTYPE> &L; // left boundaries of internal node
    vector<INDEXTYPE> &R; // right boundaries of internal node
    vector<INDEXTYPE> &D; // depths of internal node
    INDEXTYPE nodeNum;
    stool::LCPInterval<INDEXTYPE> _current_interval;
    uint64_t _current_i = 0;

public:
    LCPIntervalIterator() = default;

    LCPIntervalIterator(vector<INDEXTYPE> &_L, vector<INDEXTYPE> &_R, vector<INDEXTYPE> &_D, INDEXTYPE _nodeNum, bool isBegin) : L(_L), R(_R), D(_D)
    {
        this->nodeNum = _nodeNum;
        if (isBegin)
        {
            this->_current_interval = stool::LCPInterval<INDEXTYPE>(L[this->_current_i], R[this->_current_i], D[this->_current_i]);
        }
        else
        {
            this->_current_i = UINT64_MAX;
        }
    }

    LCPIntervalIterator &operator++()
    {
        this->_current_i++;
        if (this->_current_i < this->nodeNum)
        {
            this->_current_interval = stool::LCPInterval<INDEXTYPE>(L[this->_current_i], R[this->_current_i], D[this->_current_i]);
        }
        else
        {
            this->_current_i = UINT64_MAX;
        }

        return *this;
    }
    stool::LCPInterval<INDEXTYPE> operator*()
    {
        return this->_current_interval;
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
class LCPIntervalIteratorGenerator
{
public:
    //vector<char> &T;
    vector<INDEXTYPE> SA; // suffix array
    vector<INDEXTYPE> L;  // left boundaries of internal node
    vector<INDEXTYPE> R;  // right boundaries of internal node
    vector<INDEXTYPE> D;  // depths of internal node
    INDEXTYPE nodeNum;
    INDEXTYPE SA_first_index = UINT64_MAX;
    INDEXTYPE _size;
    LCPIntervalIterator<INDEXTYPE> *_end;

public:
    ~LCPIntervalIteratorGenerator()
    {
        delete this->_end;
    }

    LCPIntervalIteratorGenerator(vector<char> &T)
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

} // namespace stool
