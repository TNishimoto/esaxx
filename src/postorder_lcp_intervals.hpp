#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "postorder_suffix_tree_intervals.hpp"

namespace stool
{
namespace esaxx
{

template <typename INDEX = uint64_t>
class TmpLCPInterval
{
public:
    INDEX i;
    INDEX lcp;

    TmpLCPInterval();
    TmpLCPInterval(INDEX _i, INDEX _lcp) : i(_i), lcp(_lcp)
    {
    }
};

template <typename INDEX = uint64_t, typename LCP = std::vector<INDEX>>
class PostorderLCPIntervals
{

public:
    using LCP_ITERATOR = typename LCP::const_iterator;
    /*
    This iterator enumerates nodes in the suffix tree of the original input text in the postorder. 
    Each node stores its lcp interval and its depth.
  */
    //template <typename LCP_ITERATOR = std::vector<uint64_t>>
    class iterator
    {

        LCP_ITERATOR _lcp_iterator;
        INDEX _lcp_iterator_index = 0;
        INDEX _found_interval_count = 0;
        INDEX _text_size = 0;
        INDEX _next_lcp_interval_i_candidate = 0;
        INDEX _peek_stack_size = 0;

        bool is_reported_internal_nodes_i = false;

        std::stack<TmpLCPInterval<INDEX>> incompleteStack;
        LCPInterval<INDEX> _currenct_lcp_interval;

        void increment_iterators()
        {
            ++_lcp_iterator_index;
            ++_lcp_iterator;
            _next_lcp_interval_i_candidate = _lcp_iterator_index - 1;
                            is_reported_internal_nodes_i = false;
        }

    public:
        iterator() = default;
        iterator(LCP_ITERATOR &__lcp_iterator, bool isBegin, INDEX __text_size) : _lcp_iterator(std::move(__lcp_iterator)), _text_size(__text_size)
        {
            if (isBegin)
            {
                this->_currenct_lcp_interval = LCPInterval<INDEX>(this->_lcp_iterator_index, this->_lcp_iterator_index, std::numeric_limits<INDEX>::max() - 1);
                this->increment_iterators();
            }
            else
            {
                this->_currenct_lcp_interval = LCPInterval<INDEX>(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max());
            }
        }
        INDEX get_current_peek_stack_size(){
            return this->_peek_stack_size;
        }
        INDEX get_text_size()
        {
            return this->_text_size;
        }
        bool succ()
        {
            INDEX n = this->get_text_size();
            while (_lcp_iterator_index < n)
            {
                if (!is_reported_internal_nodes_i)
                {
                    auto current_lcp = *_lcp_iterator;
                    if (incompleteStack.size() > 0)
                    {
                        auto top = incompleteStack.top();

                        if (top.lcp > current_lcp)
                        {
                            this->_currenct_lcp_interval = LCPInterval<INDEX>(top.i, this->_lcp_iterator_index - 1, top.lcp);
                            incompleteStack.pop();
                            _next_lcp_interval_i_candidate = top.i;
                            return true;
                        }
                        else if (top.lcp == current_lcp)
                        {
                            is_reported_internal_nodes_i = true;
                        }
                        else
                        {
                            incompleteStack.push(TmpLCPInterval<INDEX>(_next_lcp_interval_i_candidate, current_lcp));

                            is_reported_internal_nodes_i = true;
                        }
                    }
                    else
                    {
                        if (current_lcp > 0)
                            incompleteStack.push(TmpLCPInterval<INDEX>(_next_lcp_interval_i_candidate, current_lcp));
                        is_reported_internal_nodes_i = true;
                    }
                }
                else
                {
                    if(incompleteStack.size() > this->_peek_stack_size) this->_peek_stack_size = incompleteStack.size();
                    this->_currenct_lcp_interval = LCPInterval<INDEX>(this->_lcp_iterator_index, this->_lcp_iterator_index, std::numeric_limits<INDEX>::max() - 1);
                        this->increment_iterators();
                    return true;
                }
            }
            while (incompleteStack.size() > 0)
            {
                auto top = incompleteStack.top();
                this->_currenct_lcp_interval = LCPInterval<INDEX>(top.i, this->_lcp_iterator_index - 1, top.lcp);
                incompleteStack.pop();
                return true;
            }
            if (this->_currenct_lcp_interval.lcp != 0)
            {
                this->_currenct_lcp_interval = LCPInterval<INDEX>(0, this->_lcp_iterator_index - 1, 0);
            }
            else
            {
                this->_currenct_lcp_interval = LCPInterval<INDEX>(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max());
                return false;
            }
        }
        void printStack()
        {
            std::vector<TmpLCPInterval<INDEX>> tmp;
            while (incompleteStack.size() > 0)
            {
                auto p = incompleteStack.top();
                tmp.push_back(p);
                incompleteStack.pop();
            }
            std::cout << "stack " << this->_lcp_iterator_index << " : ";
            for (int i = tmp.size() - 1; i >= 0; --i)
            {
                std::cout << "[" << tmp[i].i << "/" << tmp[i].lcp << "]";
                incompleteStack.push(tmp[i]);
            }
            std::cout << std::endl;
        }

        iterator &operator++()
        {
            succ();
            return *this;
        }
        LCPInterval<INDEX> operator*()
        {
            return this->_currenct_lcp_interval;
        }

        INDEX get_index()
        {
            return this->_found_interval_count;
        }

        bool operator!=(const iterator &rhs)
        {
            return _currenct_lcp_interval.lcp != rhs._currenct_lcp_interval.lcp;
        }
        bool isEnded()
        {
            return this->_currenct_lcp_interval.lcp == std::numeric_limits<INDEX>::max();
        }
    };

private:
    const LCP *_LCPArray;
    bool deleteFlag = false;

public:
    PostorderLCPIntervals() : deleteFlag(false)
    {
    }

    ~PostorderLCPIntervals()
    {
        if (deleteFlag)
        {
            delete _LCPArray;
        }
    }

    PostorderLCPIntervals(PostorderLCPIntervals &&obj)
    {
        this->_LCPArray = obj._LCPArray;

        this->deleteFlag = obj.deleteFlag;
        obj.deleteFlag = false;
    }
    PostorderLCPIntervals(const PostorderLCPIntervals &obj)
    {
        if (obj._SA != nullptr)
        {
            throw std::logic_error("PostorderSuffixTree instances cannot call the copy constructor.");
        }
    }

    void construct(const LCP *__lcp)
    {
        this->_LCPArray = __lcp;

        deleteFlag = false;
    }

    const LCP *get_LCP_pointer() const
    {
        return this->_LCPArray;
    }

    iterator begin() const
    {
        //using LCP_IT = decltype(this->get_LCP_pointer()->begin());
        LCP_ITERATOR lcp_beg = this->_LCPArray->begin();
        auto it = iterator(lcp_beg, true, this->_LCPArray->size());
        return it;
    }
    iterator end() const
    {
        //using LCP_IT = decltype(this->get_LCP_pointer()->begin());
        LCP_ITERATOR lcp_beg = this->_LCPArray->begin();
        auto it = iterator(lcp_beg, false, this->_LCPArray->size());
        return it;
    }

    std::vector<LCPInterval<INDEX>> to_lcp_intervals() const
    {
        std::vector<LCPInterval<INDEX>> r;

        for (auto c : *this)
        {
            r.push_back(c);
        }

        return r;
    }

    static std::vector<stool::LCPInterval<INDEX>> compute_lcp_intervals(LCP &lcpArray)
    {
        stool::esaxx::PostorderLCPIntervals<INDEX, LCP> st;
        st.construct(&lcpArray);
        std::vector<stool::LCPInterval<INDEX>> intervals = st.to_lcp_intervals();
        return intervals;
    }
};

} // namespace esaxx
} // namespace stool