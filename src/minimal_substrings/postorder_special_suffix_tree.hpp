#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "../mycode.hpp"
#include <stack>
#include "../postorder_suffix_tree.hpp"

namespace stool
{

namespace esaxx
{
// This class has information for constructing minimal substrings about a suffix tree node.
// This class stores the interval [i..j] of the node on SA, and the length of the edge between the parent and the node.
template <typename CHAR, typename INDEX>
class SpecializedLCPInterval
{
public:
    // The interval of this node on SA.
    LCPInterval<INDEX> interval;
    // The parent index of this node.
    INDEX parent;
    // If this node is a leaf, the bwtChar is the character of the bwt on this leaf.
    CHAR bwtChar;
    INDEX charRankOrID;
    SpecializedLCPInterval()
    {
    }
    SpecializedLCPInterval(LCPInterval<INDEX> _interval, INDEX _parent, CHAR _c, INDEX _charRank) : interval(_interval), parent(_parent), bwtChar(_c), charRankOrID(_charRank)
    {
    }
    static SpecializedLCPInterval<CHAR, INDEX> create_end_marker()
    {
        return SpecializedLCPInterval<CHAR, INDEX>(LCPInterval<INDEX>::create_end_marker(), std::numeric_limits<INDEX>::max(), 0, 0);
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
template <typename CHAR, typename INDEX>
struct SSTChildIntervalInfo
{
    LCPInterval<INDEX> interval;
    INDEX id;
    CHAR charRank;
    SSTChildIntervalInfo()
    {
    }
    SSTChildIntervalInfo(LCPInterval<INDEX> _interval, INDEX _id, INDEX _charRank) : interval(_interval), id(_id), charRank(_charRank)
    {
    }
};

template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename VEC = std::vector<uint64_t>>
class PostorderSSTIterator
{
    PostorderSTIterator<INDEX, VEC> _iterator;
    std::vector<CHAR> &_bwt;
    //INDEX counter_i = 0;
    INDEX current_i = 0;
    std::unordered_map<CHAR, INDEX> charRankMap;
    std::stack<SSTChildIntervalInfo<CHAR, INDEX>> childStack;
    std::queue<SpecializedLCPInterval<CHAR, INDEX>> outputQueue;
    SpecializedLCPInterval<CHAR, INDEX> _currenct_lcp_interval;

    //LCPIterator<INDEX, VEC> _lcp_forward_iterator;
    //std::queue<LCPInterval<INDEX>> parentQueue;

    //int64_t n = 0;
    bool compute_next_special_interval()
    {
        LCPInterval<INDEX> x = *_iterator;
        INDEX id = this->_iterator.get_current_i();

        while (childStack.size() > 0)
        {
            SSTChildIntervalInfo<CHAR, INDEX> top = childStack.top();
            LCPInterval<INDEX> &childInterval = top.interval;
            if (x.i <= childInterval.i && childInterval.j <= x.j)
            {
                if (childInterval.i == childInterval.j)
                {
                    outputQueue.push(SpecializedLCPInterval<CHAR, INDEX>(childInterval, id, _bwt[childInterval.i], top.charRank));
                }
                else
                {
                    outputQueue.push(SpecializedLCPInterval<CHAR, INDEX>(childInterval, id, 0, top.id));
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
            childStack.push(SSTChildIntervalInfo<CHAR, INDEX>(x, id, this->charRankMap[_bwt[x.i]]));
        }
        else
        {
            childStack.push(SSTChildIntervalInfo<CHAR, INDEX>(x, id, 0));
        }

        ++_iterator;
        //counter_i++;
        return !_iterator.isEnded();
    }
    bool succ()
    {
        while (this->outputQueue.size() == 0 && !_iterator.isEnded())
        {
            this->compute_next_special_interval();
        }
        if (_iterator.isEnded() && childStack.size() == 1)
        {
            SSTChildIntervalInfo<CHAR, INDEX> top = childStack.top();
            outputQueue.push(SpecializedLCPInterval<CHAR, INDEX>(top.interval, std::numeric_limits<INDEX>::max(), 0, top.id));
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
            _currenct_lcp_interval = SpecializedLCPInterval<CHAR, INDEX>::create_end_marker();

            return false;
        }
        return true;
    }

public:
    PostorderSSTIterator() = default;

    PostorderSSTIterator(std::vector<CHAR> &__bwt, PostorderSTIterator<> &__iterator, bool isBegin) : _iterator(__iterator), _bwt(__bwt)
    {
        if (isBegin)
        {
            this->succ();
        }
        else
        {
            this->_currenct_lcp_interval = SpecializedLCPInterval<CHAR, INDEX>::create_end_marker();
        }
    }

    //bool takeFront(LCPInterval &outputInterval);

    PostorderSSTIterator &operator++()
    {
        this->succ();
        current_i++;
        return *this;
    }
    SpecializedLCPInterval<CHAR, INDEX> operator*()
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

    static PostorderSSTIterator<CHAR, INDEX, VEC> constructIterator(std::vector<CHAR> &__bwt, VEC &_SA, VEC &_LCPArray)
    {
        if (_SA[0] != _SA.size() - 1)
        {
            throw std::logic_error("error!");
        }
        PostorderSTIterator<INDEX, VEC> st(&_SA, &_LCPArray, true);
        return PostorderSSTIterator<CHAR, INDEX, VEC>(__bwt, st, true);
    }

    bool isEnded()
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
} // namespace esaxx
} // namespace stool