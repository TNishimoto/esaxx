#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "mycode.hpp"
#include <stack>
#include "postorder_suffix_tree.hpp"

namespace stool
{

// This class has information about a node in the suffix tree of an input string.
// This class stores the interval [i..j] of the node on SA, and the length of the edge between the parent and the node.
template <typename INDEX = uint64_t>
class SpecializedLCPInterval
{
public:
    INDEX i;
    INDEX j;
    INDEX parentEdgeLength;
    char bwtChar;
    SpecializedLCPInterval()
    {
    }
    SpecializedLCPInterval(INDEX _i, INDEX _j, INDEX _lcp, char _c) : i(_i), j(_j), parentEdgeLength(_lcp), bwtChar(_c)
    {
    }

    std::string toString()
    {
        return "[" + std::to_string(i) + ", " + std::to_string(j) + ", " + std::to_string(parentEdgeLength) + ", " + std::string(1, bwtChar) + "]";
    }

  bool isEnd(){
    return this->i == std::numeric_limits<INDEX>::max() && this->j == std::numeric_limits<INDEX>::max() && this->parentEdgeLength == std::numeric_limits<INDEX>::max();
  }
};

template <typename CHAR = char, typename INDEX = uint64_t, typename VEC = std::vector<uint64_t>>
class PostorderSSTIterator
{
    PostorderSTIterator<INDEX, VEC> _iterator;
    vector<CHAR> &_bwt;
    INDEX counter_i = 0;

    //LCPIterator<INDEX, VEC> _lcp_forward_iterator;
    std::stack<std::pair<INDEX, INDEX>> childStack;
    //std::queue<LCPInterval<INDEX>> parentQueue;
    std::queue<SpecializedLCPInterval<INDEX>> outputQueue;

    SpecializedLCPInterval<INDEX> _currenct_lcp_interval;

    //int64_t n = 0;
    bool compute_next_special_interval()
    {
        LCPInterval<INDEX> x = *_iterator;
        ++_iterator;
        counter_i++;

        while (childStack.size() > 0)
        {
            std::pair<INDEX, INDEX> top = childStack.top();
            if (x.i <= top.first && top.second <= x.j)
            {
                if(top.first == top.second){

                    outputQueue.push(SpecializedLCPInterval<INDEX>(top.first, top.second, x.lcp, _bwt[top.first]));
                }else{
                    outputQueue.push(SpecializedLCPInterval<INDEX>(top.first, top.second, x.lcp, 0));
                }
                childStack.pop();
            }
            else
            {
                break;
            }
        }        
        childStack.push(std::pair<INDEX, INDEX>(x.i, x.j));

        return !_iterator.end();
    }
    bool succ()
    {
        while (this->outputQueue.size() == 0 && !_iterator.end())
        {
            this->compute_next_special_interval();
        }
        if(_iterator.end() && childStack.size() == 1){
            std::pair<INDEX, INDEX> top = childStack.top();
            outputQueue.push(SpecializedLCPInterval<INDEX>(top.first, top.second, std::numeric_limits<INDEX>::max(),0));
            childStack.pop();

        }

        if (this->outputQueue.size() > 0)
        {
            SpecializedLCPInterval<INDEX> sli = this->outputQueue.front();
            _currenct_lcp_interval.i = sli.i;
            _currenct_lcp_interval.j = sli.j;
            _currenct_lcp_interval.bwtChar = sli.bwtChar;            
            _currenct_lcp_interval.parentEdgeLength = sli.parentEdgeLength;
            this->outputQueue.pop();
            return true;
        }
        else
        {
            _currenct_lcp_interval.i = std::numeric_limits<INDEX>::max();
            _currenct_lcp_interval.j = std::numeric_limits<INDEX>::max();
            _currenct_lcp_interval.parentEdgeLength = std::numeric_limits<INDEX>::max();
            _currenct_lcp_interval.bwtChar = 0;

            return false;
        }
        return true;
    }

public:
    PostorderSSTIterator() = default;
    
    PostorderSSTIterator(vector<CHAR> &__bwt, PostorderSTIterator<> &__iterator, bool isBegin) : _bwt(__bwt), _iterator(__iterator)
    {
        if (isBegin)
        {
            this->succ();
        }
        else
        {
            this->_currenct_lcp_interval = SpecializedLCPInterval<INDEX>(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(),0);
        }
    }
    

    //bool takeFront(LCPInterval &outputInterval);

    PostorderSSTIterator &operator++()
    {
        this->succ();
        return *this;
    }
    SpecializedLCPInterval<INDEX> operator*()
    {
        return this->_currenct_lcp_interval;
    }
    bool operator!=(const PostorderSSTIterator &rhs)
    {
        return _currenct_lcp_interval.parentEdgeLength != rhs._currenct_lcp_interval.parentEdgeLength;
    }

    static PostorderSSTIterator<CHAR,INDEX,VEC> constructIterator(vector<CHAR> &__bwt, VEC &_SA, VEC &_LCPArray){
        PostorderSTIterator<INDEX,VEC> st(&_SA, &_LCPArray, true);
        return PostorderSSTIterator<CHAR,INDEX,VEC>(__bwt,st, true);
    }

    bool isEnd(){
        return _currenct_lcp_interval.isEnd(); 
    }
};
/*
template <typename INDEX = uint64_t, typename VEC = std::vector<INDEX>>
class PostorderSpecialSuffixTree
{

    const VEC *_SA;
    const VEC *_LCPArray;
    bool deleteFlag = false;

public:
    PostorderSpecialSuffixTree(VEC &&__SA, VEC &&__LCPArray) : _SA(new VEC(std::move(__SA))), _LCPArray(new VEC(std::move(__LCPArray))), deleteFlag(true)
    {
    }
    PostorderSpecialSuffixTree() : deleteFlag(false)
    {
    }

    ~PostorderSpecialSuffixTree()
    {
        if (deleteFlag)
            delete _SA;
        delete _LCPArray;
    }

    PostorderSpecialSuffixTree(PostorderSpecialSuffixTree &&obj)
    {
        this->_SA = obj._SA;
        this->_LCPArray = obj._LCPArray;

        this->deleteFlag = obj.deleteFlag;
        obj.deleteFlag = false;
    }
    PostorderSpecialSuffixTree(const PostorderSpecialSuffixTree &obj)
    {
        if (obj._SA != nullptr)
        {
            throw std::logic_error("PostorderSpecialSuffixTree instances cannot call the copy constructor.");
        }
    }

    void set(VEC &&__SA, VEC &&__LCPArray)
    {
        this->_SA = new VEC(std::move(__SA));
        this->_LCPArray = new VEC(std::move(__LCPArray));

        deleteFlag = true;
    }

    PostorderSTIterator<INDEX, VEC> begin() const
    {
        //LCPIterator<INDEX, VEC> lcp_it = this->_lcp_generator->begin();
        auto it = PostorderSTIterator<INDEX, VEC>(this->_SA, this->_LCPArray, true);
        return it;
    }
    PostorderSTIterator<INDEX, VEC> end() const
    {
        //LCPIterator<INDEX, VEC> lcp_it = this->_lcp_generator->begin();
        auto it = PostorderSTIterator<INDEX, VEC>(this->_SA, this->_LCPArray, false);
        return it;
    }
    std::vector<LCPInterval<INDEX>> to_lcp_intervals() const
    {
        std::vector<LCPInterval<INDEX>> r;
        for (LCPInterval<INDEX> c : *this)
        {
            r.push_back(c);
        }
        return r;
    }
};
*/
} // namespace stool