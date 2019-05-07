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
    //INDEXTYPE SA_first_index = 0;
    //INDEXTYPE _size;

public:
    LCPIntervalIterator() = default;

    LCPIntervalIterator(vector<INDEXTYPE> &_L, vector<INDEXTYPE> &_R, vector<INDEXTYPE> &_D, INDEXTYPE _nodeNum,bool isBegin) : L(_L), R(_R), D(_D)
    {
        this->nodeNum = _nodeNum;
        //this->SA_first_index = _SA_first_index;
        //this->_size = _n;
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
    ~LCPIntervalIteratorGenerator(){
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

} // namespace stool
