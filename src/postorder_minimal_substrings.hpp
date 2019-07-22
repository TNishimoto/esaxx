#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "mycode.hpp"
#include <stack>
#include "postorder_special_suffix_tree.hpp"


namespace stool
{

 
template <typename INDEX = uint64_t, typename VEC = std::vector<uint64_t>>
class PostorderMinimalSubstringsIterator
{
  PostorderSSTIterator<INDEX, VEC> _iterator;  
  INDEX counter_i = 0;


  bool succ()
  {
    while (true)
    {
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
  PostorderSTIterator() = default;
  PostorderSTIterator(const VEC *_SA, const VEC *_LCPArray, bool isBegin) : SA(_SA), LCPArray(_LCPArray)
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
    return (*SA).size();
  }

  //bool takeFront(LCPInterval &outputInterval);

  PostorderSTIterator &operator++()
  {
    this->succ();
    return *this;
  }
  LCPInterval<INDEX> operator*()
  {
    return this->_currenct_lcp_interval;
  }
  bool operator!=(const PostorderSTIterator &rhs)
  {
    return _currenct_lcp_interval.lcp != rhs._currenct_lcp_interval.lcp;
  }
  bool end(){
    return this->_currenct_lcp_interval.lcp == std::numeric_limits<INDEX>::max();
  }
};

} // namespace stool