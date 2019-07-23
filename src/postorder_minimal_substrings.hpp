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

template <typename INDEX = uint64_t>
class MinimalSubstringCandidates
{
public:
  INDEX count;
  INDEX rank;
  vector<std::pair<INDEX, INDEX>> intervals;

  MinimalSubstringCandidates()
  {
  }
  MinimalSubstringCandidates(INDEX _count, INDEX _rank) : count(_count), rank(_rank)
  {
    this->intervals.push_back(std::pair<INDEX,INDEX>( _rank, _count) );
  }
};

template <typename CHAR = char, typename INDEX = uint64_t>
std::unordered_map<CHAR, INDEX> constructCMap(vector<CHAR> &text)
{
  std::unordered_map<CHAR, INDEX> C;
  vector<CHAR> chars;

  for (uint64_t i = 0; i < text.size(); i++)
  {
      if (C.find(text[i]) == C.end())
      {
        C[text[i]] = 1;
        chars.push_back(text[i]);
      }
      else
      {
        C[text[i]]++;
      }
  }
  std::sort(chars.begin(), chars.end());
  INDEX counter = 0;
  for (INDEX i = 0; i < chars.size(); i++)
  {
    INDEX tmp = counter;
    counter += C[chars[i]]; 
    C[chars[i]] = tmp;
  }
  return C;
}

template <typename CHAR = char, typename INDEX = uint64_t>
class IntervalInfoForMinimalSubstrings
{
public:
  //The previous characters in the interval instance, i.e., Chars(bwt[i..j]), where interval = [i..j].
  std::unordered_map<CHAR, MinimalSubstringCandidates<INDEX>> map;

  LCPInterval<INDEX> interval;

  void addChar(CHAR c, INDEX count, INDEX rank)
  {
    if (this->map.find(c) == this->map.end())
    {
      this->map[c] = MinimalSubstringCandidates<INDEX>(count, rank);
    }
    else
    {
      auto &it = this->map[c];
      it.count = count + it.count;
      it.rank = rank < it.rank ? rank : it.rank;
      it.intervals.push_back(std::pair<INDEX, INDEX>(rank, count));
    }
  }

  void add(IntervalInfoForMinimalSubstrings<CHAR, INDEX> &child)
  {
    for (auto &it : child.map)
    {
      this->addChar(it.first, it.second.count, it.second.rank);
    }
  }
  bool take(std::unordered_map<CHAR, INDEX> &first_occcurrence_map_on_F, std::queue<LCPInterval<INDEX>> &output)
  {
    INDEX lcp = interval.lcp + 2;
    bool b = false;
    for (auto &it : this->map)
    {
      const CHAR &c = it.first;
      if (it.second.intervals.size() >= 2)
      {
        for (auto &ms : it.second.intervals)
        {
          INDEX i = first_occcurrence_map_on_F[c] + ms.first;
          output.push(LCPInterval<INDEX>(i, i + ms.second - 1, lcp));
          b = true;
        }
      }
    }
    return b;
  }
};

template <typename CHAR = char, typename INDEX = uint64_t, typename VEC = std::vector<uint64_t>>
class MinimalSubstringIterator
{
  PostorderSSTIterator<CHAR, INDEX, VEC> _iterator;
  std::unordered_map<INDEX, IntervalInfoForMinimalSubstrings<CHAR, INDEX>> map;
  std::queue<LCPInterval<INDEX>> outputQueue;
  std::unordered_map<CHAR, INDEX> first_occcurrence_map_on_F;
  LCPInterval<INDEX> _currenct_lcp_interval;
  INDEX counter_i = 0;

  bool add(SpecializedLCPInterval<INDEX> &interval_info, INDEX id)
  {

    if (this->map.find(interval_info.parent) == this->map.end())
    {
      this->map[interval_info.parent] = IntervalInfoForMinimalSubstrings<CHAR,INDEX>();
    }
    IntervalInfoForMinimalSubstrings<CHAR, INDEX> &parent_info = this->map[interval_info.parent];

    if (interval_info.interval.i == interval_info.interval.j)
    {
      parent_info.addChar(interval_info.bwtChar, 1, interval_info.charRank);
      return false;
    }
    else
    {
      IntervalInfoForMinimalSubstrings<CHAR, INDEX> &item_info = this->map[id];
      bool b = item_info.take(first_occcurrence_map_on_F, this->outputQueue);
      parent_info.add(item_info);
      this->map.erase(id);
      return b;
    }
  }

  bool succ()
  {
    bool b = false;
    while(!this->_iterator.isEnd() ){
      SpecializedLCPInterval<INDEX> sintv = *this->_iterator;
      b = this->add(sintv, this->_iterator.get_current_i() );
      ++this->_iterator;
      if(b) break;
    }
    return b;
  }

public:
  MinimalSubstringIterator() = default;
  MinimalSubstringIterator(vector<CHAR> &__bwt, PostorderSSTIterator<CHAR, INDEX,VEC> &__iterator) : _iterator(__iterator)
  {
    this->first_occcurrence_map_on_F = constructCMap(__bwt);
    this->succ();
    this->_currenct_lcp_interval = this->outputQueue.front();
    this->outputQueue.pop();
  }
  MinimalSubstringIterator &operator++()
  {
    bool b = this->succ();
    if(b){
    this->_currenct_lcp_interval = this->outputQueue.front();
    this->outputQueue.pop();
    }
    else{
    this->_currenct_lcp_interval = LCPInterval<INDEX>::create_end_marker() ;
    }
    return *this;
  }

  LCPInterval<INDEX> operator*()
  {
    return this->_currenct_lcp_interval;
  }
  
  bool end()
  {
    return this->_currenct_lcp_interval.is_special_marker();
  }
  
};

} // namespace stool