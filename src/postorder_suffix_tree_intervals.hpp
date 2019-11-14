#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "postorder_suffix_tree.hpp"

namespace stool
{
namespace esaxx
{

template <typename INDEX = uint64_t, typename SA = std::vector<uint64_t>, typename LCP = std::vector<uint64_t>>
class PostorderSuffixTreeIntervals
{

  public:
  /*
    This iterator enumerates nodes in the suffix tree of the original input text in the postorder. 
    Each node stores its lcp interval and its depth.
  */
  template <typename SA_ITERATOR = std::vector<uint64_t>, typename LCP_ITERATOR = std::vector<uint64_t>>
  class iterator
  {
    //using SA_ITERATOR = decltype(this->get_SA_ponter()->begin() );
    //using LCP_ITERATOR = decltype(LCP::begin() );

    SA_ITERATOR _sa_iterator;
    LCP_ITERATOR _lcp_iterator;
    INDEX counter_i = 0;
    INDEX current_i = 0;
    INDEX _text_size = 0;

    //LCPIterator<INDEX, VEC> _lcp_forward_iterator;
    std::stack<IncompleteLCPInterval<INDEX>> incompleteStack;
    std::queue<IncompleteLCPInterval<INDEX>> outputQueue;
    LCPInterval<INDEX> _currenct_lcp_interval;

    //int64_t n = 0;
    bool report_next_interval_at_previous_i(IncompleteLCPInterval<INDEX> &fst)
    {

      if (this->incompleteStack.size() == 0)
        return false;
      IncompleteLCPInterval<INDEX> second = incompleteStack.top();
      incompleteStack.pop();

      if (second.i == second.j && second.i_lcp != second.lcp && second.lcp > fst.i_lcp)
      {
        this->outputQueue.push(second);
        second.lcp = second.i_lcp;
        this->incompleteStack.push(second);
        //std::cout << "#1" << std::endl;

        return true;
      }

      if (second.i_lcp > fst.i_lcp)
      {
        assert(incompleteStack.size() > 0);
        IncompleteLCPInterval<INDEX> &newInterval = second;
        newInterval.lcp = second.i_lcp;

        while (incompleteStack.size() > 0)
        {
          IncompleteLCPInterval<INDEX> third = incompleteStack.top();
          newInterval.i = third.i;
          newInterval.i_lcp = third.i_lcp;
          incompleteStack.pop();
          if (newInterval.lcp != third.i_lcp)
          {
            break;
          }
        }
        //assert(newi != newj);
        this->outputQueue.push(newInterval);
        incompleteStack.push(newInterval);
        return true;
      }
      else
      {
        //std::cout << "#3" << std::endl;
        incompleteStack.push(second);
        return false;
      }
    }
    void increment_iterators(){
       ++counter_i;
       ++_sa_iterator;
       ++_lcp_iterator;
    }

    bool compute_next_lcp_Interval()
    {
      INDEX n = this->get_text_size();

      while (counter_i <= n)
      {
        INDEX sa_value = n, lcp_value = 0;
        if (counter_i < n)
        {
          sa_value = *(_sa_iterator);
          lcp_value = *(_lcp_iterator);
        }
        IncompleteLCPInterval<INDEX> fst = IncompleteLCPInterval<INDEX>(counter_i, counter_i, n - sa_value, lcp_value);
        bool isReported = this->report_next_interval_at_previous_i(fst);
        if (isReported)
        {
          return true;
        }
        else
        {
          this->incompleteStack.push(fst);
          this->increment_iterators();
          //++counter_i;
        }
      }
      if (counter_i == n + 1)
      {
        this->outputQueue.push(IncompleteLCPInterval<INDEX>(0, n - 1, 0, 0));
        //++counter_i;
        this->increment_iterators();
          
        return true;
      }
      else
      {
        return false;
      }
    }
    bool succ()
    {
      while (true)
      {
        //assert(this->outputQueue.size() < this->get_text_size() * 3);
        if (this->outputQueue.size() > 0)
        {
          IncompleteLCPInterval<INDEX> sli = this->outputQueue.front();
          _currenct_lcp_interval.i = sli.i;
          _currenct_lcp_interval.j = sli.j;
          _currenct_lcp_interval.lcp = sli.lcp;
          this->outputQueue.pop();
          return true;
        }
        else
        {
          bool b = this->compute_next_lcp_Interval();
          if (!b)
          {
            _currenct_lcp_interval.i = std::numeric_limits<INDEX>::max();
            _currenct_lcp_interval.j = std::numeric_limits<INDEX>::max();
            _currenct_lcp_interval.lcp = std::numeric_limits<INDEX>::max();
            return false;
          }
        }
      }
      return true;
    }

  public:
    iterator() = default;
    iterator(SA_ITERATOR &__sa_iterator, LCP_ITERATOR &__lcp_iterator, bool isBegin, INDEX __text_size) : _sa_iterator(std::move(__sa_iterator) ), _lcp_iterator(std::move(__lcp_iterator) ), _text_size(__text_size)
    {
      if (isBegin)
      {
        this->succ();
      }
      else
      {
        this->_currenct_lcp_interval = LCPInterval<INDEX>(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max());
      }
    }
    INDEX get_text_size()
    {
      //return (*_SA).size();
      return this->_text_size;
    }

    //bool takeFront(LCPInterval &outputInterval);

    iterator &operator++()
    {
      this->succ();
      this->current_i++;
      return *this;
    }
    LCPInterval<INDEX> operator*()
    {
      return this->_currenct_lcp_interval;
    }

    INDEX get_current_i()
    {
      return this->current_i;
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
  const SA *_SA;
  const LCP *_LCPArray;
  //const ForwardLCPArray<INDEX, VEC> *_lcp_generator;
  bool deleteFlag = false;

public:
  /*
  PostorderSuffixTreeIntervals(SAITERATOR &&__SA, VEC &&__LCPArray) : _SAITERATOR(new SAITERATOR(std::move(__SA))), _LCPArray(new VEC(std::move(__LCPArray))), deleteFlag(true)
  {
  }
  */
  PostorderSuffixTreeIntervals() : deleteFlag(false)
  {
  }

  ~PostorderSuffixTreeIntervals()
  {
    if (deleteFlag)
    {
      delete _SA;
      delete _LCPArray;
    }
  }

  PostorderSuffixTreeIntervals(PostorderSuffixTreeIntervals &&obj)
  {
    this->_SA = obj._SA;
    this->_LCPArray = obj._LCPArray;

    this->deleteFlag = obj.deleteFlag;
    obj.deleteFlag = false;
  }
  PostorderSuffixTreeIntervals(const PostorderSuffixTreeIntervals &obj)
  {
    if (obj._SA != nullptr)
    {
      throw std::logic_error("PostorderSuffixTree instances cannot call the copy constructor.");
    }
  }
  /*
  void set(SA &&__SA, LCP &&__LCPArray)
  {
    this->_SA = new SA(std::move(__SA));
    this->_LCPArray = new LCP(std::move(__LCPArray));

    deleteFlag = true;
  }
  */
 /*
  void set(SA &__SA, LCP &__LCPArray)
  {
    this->_SA = &__SA;
    this->_LCPArray = &__LCPArray;

    deleteFlag = false;
  }
  */

  void construct(const SA* __sa,const LCP* __lcp)
  {
    this->_SA = __sa;
    this->_LCPArray = __lcp;

    deleteFlag = false;
  }


  const SA* get_SA_pointer() const {
    return this->_SA;
  }
  const LCP* get_LCP_pointer() const {
    return this->_LCPArray;
  }

  auto begin() const -> iterator< decltype(this->get_SA_pointer()->begin() ), decltype(this->get_LCP_pointer()->begin() )>
  {
    using SA_IT = decltype(this->get_SA_pointer()->begin() );
    using LCP_IT = decltype(this->get_LCP_pointer()->begin() );

    //LCPIterator<INDEX, VEC> lcp_it = this->_lcp_generator->begin();
    SA_IT sa_beg = this->_SA->begin();
    LCP_IT lcp_beg = this->_LCPArray->begin();
    auto it = iterator<SA_IT, LCP_IT>(sa_beg, lcp_beg, true, this->_SA->size() );
    return it;
  }
  auto end() const -> iterator< decltype(this->get_SA_pointer()->begin() ), decltype(this->get_LCP_pointer()->begin() )>
  {    
    //LCPIterator<INDEX, VEC> lcp_it = this->_lcp_generator->begin();
    decltype(this->_SA->begin()) sa_beg = this->_SA->begin();
    decltype(this->_LCPArray->begin()) lcp_beg = this->_LCPArray->begin();

    auto it = iterator<decltype(this->_SA->begin()), decltype(this->_LCPArray->begin())>(sa_beg, lcp_beg, false, this->_SA->size());
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
  

};

template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename SA = std::vector<INDEX>, typename LCP = std::vector<INDEX>>
std::vector<stool::LCPInterval<INDEX>> compute_preorder_lcp_intervals(std::vector<CHAR> &text, SA &sa, LCP &lcpArray)
{
  //std::vector<INDEX> lcpArray = stool::constructLCP<CHAR, INDEX>(text, sa);
  stool::esaxx::PostorderSuffixTreeIntervals<INDEX, SA, LCP> st;
  st.construct(&sa, &lcpArray);
  std::vector<stool::LCPInterval<INDEX>> intervals = st.to_lcp_intervals();

  std::sort(
      intervals.begin(),
      intervals.end(),
      stool::LCPIntervalPreorderComp<INDEX>());

  return intervals;
}

template <typename CHAR = uint8_t, typename INDEX = uint64_t>
std::vector<stool::LCPInterval<INDEX>> compute_preorder_lcp_intervals(std::vector<CHAR> &text, std::vector<INDEX> &sa)
{
  //assert(sa.size() != 0);
  std::vector<INDEX> lcpArray = stool::constructLCP<CHAR, INDEX>(text, sa);
  stool::esaxx::PostorderSuffixTreeIntervals<INDEX> st;
  st.construct(&sa, &lcpArray);
  std::vector<stool::LCPInterval<INDEX>> intervals = st.to_lcp_intervals();

  std::sort(
      intervals.begin(),
      intervals.end(),
      stool::LCPIntervalPreorderComp<INDEX>());

  return intervals;
}

template <typename CHAR = uint8_t, typename INDEX = uint64_t>
std::vector<stool::LCPInterval<INDEX>> compute_preorder_lcp_intervals(std::vector<CHAR> &text)
{
  std::vector<INDEX> sa = stool::construct_suffix_array(text);
  return compute_preorder_lcp_intervals(text, sa);
}

} // namespace esaxx
} // namespace stool