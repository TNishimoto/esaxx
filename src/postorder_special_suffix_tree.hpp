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
    LCPInterval<INDEX> interval;
    INDEX parent;
    char bwtChar;
    INDEX charRankOrID;
    SpecializedLCPInterval()
    {
    }
    SpecializedLCPInterval(LCPInterval<INDEX> _interval, INDEX _parent, char _c, INDEX _charRank) : interval(_interval), parent(_parent), bwtChar(_c), charRankOrID(_charRank)
    {
    }
    static SpecializedLCPInterval<INDEX> create_end_marker()
    {
        return SpecializedLCPInterval<INDEX>(LCPInterval<INDEX>::create_end_marker(), std::numeric_limits<INDEX>::max(), 0, 0);
    }

    std::string to_string()
    {
        return "[" + this->interval.to_string() + "," + std::to_string(this->parent) + ", " + std::string(1, bwtChar) + ", " + std::to_string(this->charRankOrID) + "]";
    }

    bool isEnd()
    {
        return this->interval.is_special_marker() && this->parent == std::numeric_limits<INDEX>::max() && this->bwtChar == 0;
    }
};
template <typename INDEX = uint64_t>
struct SSTChildIntervalInfo
{
    LCPInterval<INDEX> interval;
    INDEX id;
    char charRank;
    SSTChildIntervalInfo()
    {
    }
    SSTChildIntervalInfo(LCPInterval<INDEX> _interval, INDEX _id, INDEX _charRank) : interval(_interval), id(_id), charRank(_charRank)
    {
    }
};

template <typename CHAR = char, typename INDEX = uint64_t, typename VEC = std::vector<uint64_t>>
class PostorderSSTIterator
{
    PostorderSTIterator<INDEX, VEC> _iterator;
    vector<CHAR> &_bwt;
    //INDEX counter_i = 0;
    INDEX current_i = 0;
    std::unordered_map<CHAR, INDEX> charRankMap;
    std::stack<SSTChildIntervalInfo<INDEX>> childStack;
    std::queue<SpecializedLCPInterval<INDEX>> outputQueue;
    SpecializedLCPInterval<INDEX> _currenct_lcp_interval;

    //LCPIterator<INDEX, VEC> _lcp_forward_iterator;
    //std::queue<LCPInterval<INDEX>> parentQueue;

    //int64_t n = 0;
    bool compute_next_special_interval()
    {
        LCPInterval<INDEX> x = *_iterator;
        INDEX id = this->_iterator.get_current_i();

        while (childStack.size() > 0)
        {
            SSTChildIntervalInfo<INDEX> top = childStack.top();
            LCPInterval<INDEX> &childInterval = top.interval;
            if (x.i <= childInterval.i && childInterval.j <= x.j)
            {
                if (childInterval.i == childInterval.j)
                {
                    outputQueue.push(SpecializedLCPInterval<INDEX>(childInterval, id, _bwt[childInterval.i], top.charRank));
                }
                else
                {
                    outputQueue.push(SpecializedLCPInterval<INDEX>(childInterval, id, 0, top.id ));
                }
                childStack.pop();
            }
            else
            {
                break;
            }
        }
        if (x.i == x.j)
        {
            if (this->charRankMap.find(_bwt[x.i]) == this->charRankMap.end())
            {
                this->charRankMap[_bwt[x.i]] = 0;
            }
            else
            {
                this->charRankMap[_bwt[x.i]]++;
            }
            childStack.push(SSTChildIntervalInfo<INDEX>(x,id, this->charRankMap[_bwt[x.i]]));
        }else{
            childStack.push(SSTChildIntervalInfo<INDEX>(x,id, 0));
        }

        ++_iterator;
        //counter_i++;
        return !_iterator.end();
    }
    bool succ()
    {
        while (this->outputQueue.size() == 0 && !_iterator.end())
        {
            this->compute_next_special_interval();
        }
        if (_iterator.end() && childStack.size() == 1)
        {
            SSTChildIntervalInfo<INDEX> top = childStack.top();
            outputQueue.push(SpecializedLCPInterval<INDEX>(top.interval, std::numeric_limits<INDEX>::max(), 0, top.id));
            childStack.pop();
        }

        if (this->outputQueue.size() > 0)
        {
            _currenct_lcp_interval = this->outputQueue.front();
            this->outputQueue.pop();
            return true;
        }
        else
        {
            _currenct_lcp_interval = SpecializedLCPInterval<INDEX>::create_end_marker();

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
            this->_currenct_lcp_interval = SpecializedLCPInterval<INDEX>::create_end_marker();
        }
    }

    //bool takeFront(LCPInterval &outputInterval);

    PostorderSSTIterator &operator++()
    {
        this->succ();
        current_i++;
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
    INDEX get_current_i()
    {
        return this->current_i;
    }

    static PostorderSSTIterator<CHAR, INDEX, VEC> constructIterator(vector<CHAR> &__bwt, VEC &_SA, VEC &_LCPArray)
    {
        PostorderSTIterator<INDEX, VEC> st(&_SA, &_LCPArray, true);
        return PostorderSSTIterator<CHAR, INDEX, VEC>(__bwt, st, true);
    }

    bool isEnd()
    {
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