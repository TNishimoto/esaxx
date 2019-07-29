#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <stack>
#include "postorder_special_suffix_tree.hpp"

namespace stool
{
namespace esaxx
{

template <typename INDEX = uint64_t>
struct LCPIntervalComp
{
  bool operator()(LCPInterval<INDEX> &x, LCPInterval<INDEX> &y)
  {
    if (x.i == y.i)
    {
      if (x.j == y.j)
      {
        return x.lcp < y.lcp;
      }
      else
      {
        return x.j > y.j;
      }
    }
    else
    {
      return x.i < y.i;
    }
  }
};

template <typename INDEX = uint64_t>
struct VRInfo
{
  INDEX count;
  INDEX rank;
  INDEX vrOcc;
  VRInfo();
  VRInfo(INDEX _rank, INDEX _count, INDEX _vrOcc) : count(_count), rank(_rank), vrOcc(_vrOcc)
  {
  }
};

template <typename INDEX = uint64_t>
class MinimalSubstringCandidates
{
public:
  INDEX count;
  INDEX rank;
  std::vector<VRInfo<INDEX>> intervals;

  MinimalSubstringCandidates()
  {
  }
  MinimalSubstringCandidates(INDEX _rank, INDEX _count, INDEX _vrOcc) : count(_count), rank(_rank)
  {
    this->intervals.push_back(VRInfo<INDEX>(_rank, _count, _vrOcc));
  }
  void print(char c)
  {
    std::cout << "Character: " << std::string(1, c) << ", rank: " << rank << ",count: " << count << ", ";
    std::cout << "intervals: ";
    for (auto &it : this->intervals)
    {
      std::cout << "[rank: " << it.rank << ", count: " << it.count << "]";
    }
    std::cout << std::endl;
  }
};

template <typename CHAR = uint8_t, typename INDEX = uint64_t>
std::unordered_map<CHAR, INDEX> constructCMap(std::vector<CHAR> &text)
{
  std::unordered_map<CHAR, INDEX> C;
  std::vector<CHAR> chars;

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
  if (chars[0] != 0)
  {
    assert(false);
    throw std::logic_error("The first character of the bwt string must be '0'");
  }
  INDEX counter = 0;
  for (INDEX i = 0; i < chars.size(); i++)
  {
    INDEX tmp = counter;
    counter += C[chars[i]];
    C[chars[i]] = tmp;
  }
  return C;
}

template <typename CHAR = uint8_t, typename INDEX = uint64_t>
class IntervalInfoForMinimalSubstrings
{
public:
  //The previous characters in the interval instance, i.e., Chars(bwt[i..j]), where interval = [i..j].
  std::unordered_map<CHAR, MinimalSubstringCandidates<INDEX>> map;

  LCPInterval<INDEX> interval;
  INDEX occ = 0;

  void addChar(CHAR c, INDEX count, INDEX rank, INDEX vrOcc)
  {
    //std::cout << "add: " << std::string(1,c) << "/" << rank << "/" << count << "/" << vrOcc << std::endl;
    if (this->map.find(c) == this->map.end())
    {
      this->map[c] = MinimalSubstringCandidates<INDEX>(rank, count, vrOcc);
    }
    else
    {
      auto &it = this->map[c];
      it.count = count + it.count;
      it.rank = rank < it.rank ? rank : it.rank;
      it.intervals.push_back(VRInfo<INDEX>(rank, count, vrOcc));
    }
    this->occ += count;
  }

  void add(IntervalInfoForMinimalSubstrings<CHAR, INDEX> &child)
  {
    for (auto &it : child.map)
    {
      this->addChar(it.first, it.second.count, it.second.rank, child.occ);
    }
  }
  bool take(std::unordered_map<CHAR, INDEX> &first_occcurrence_map_on_F, std::queue<LCPInterval<INDEX>> &output)
  {
    INDEX lcp = interval.lcp + 2;
    bool b = false;
    for (auto &it : this->map)
    {
      const CHAR &c = it.first;
      INDEX lvOcc = it.second.count;
      //if (it.second.intervals.size() >= 2)
      //{
      for (auto &ms : it.second.intervals)
      {
        INDEX lvrOcc = ms.count;
        INDEX vrOcc = ms.vrOcc;
#ifdef DEBUG_PRINT
        std::cout << std::string(1, c) << ">>" << interval.to_string() << std::endl;
        std::cout << "check: lvOcc: " << lvOcc << "/"
                  << "lvrOcc: " << ms.count << ", vrOcc:" << vrOcc << std::endl;
        std::cout << std::string(1, c) << ", rank = " << ms.rank << "@" << first_occcurrence_map_on_F[c] << std::endl;
#endif
        if (lvOcc != lvrOcc && lvrOcc != vrOcc)
        {
          INDEX i = first_occcurrence_map_on_F[c] + ms.rank;
          output.push(LCPInterval<INDEX>(i, i + ms.count - 1, lcp));

          if (c == 0)
          {
            std::cout << LCPInterval<INDEX>(i, i + ms.count - 1, lcp).to_string() << std::endl;

            std::cout << (char)c << std::endl;

            assert(false);
          }

#ifdef DEBUG_PRINT
          std::cout << "output!";
          auto outputInterval = LCPInterval<INDEX>(i, i + ms.count - 1, lcp);
          std::cout << interval.to_string() << "->" << outputInterval.to_string() << std::endl;
#endif

          b = true;
        }
      }
      //}
      if (interval.lcp == 0)
      {
        INDEX i = first_occcurrence_map_on_F[c];
        output.push(LCPInterval<INDEX>(i, i + it.second.count - 1, 1));
      }
    }

    return b;
  }

  void print(INDEX id)
  {
    std::cout << "--- id: " << id << " -------------------" << std::endl;
    //if()
    //std::cout << this->interval.to_string() << std::endl;
    for (auto &it : this->map)
    {
      const CHAR &c = it.first;
      it.second.print((char)c);
    }
    std::cout << ">>>-------------------" << std::endl;
  }
};

template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename VEC = std::vector<uint64_t>>
class MinimalSubstringIterator
{
  PostorderSSTIterator<CHAR, INDEX, VEC> _iterator;
  std::unordered_map<INDEX, IntervalInfoForMinimalSubstrings<CHAR, INDEX>> map;
  std::queue<LCPInterval<INDEX>> outputQueue;
  std::unordered_map<CHAR, INDEX> first_occcurrence_map_on_F;
  LCPInterval<INDEX> _currenct_lcp_interval;
  INDEX counter_i = 0;

  bool add(SpecializedLCPInterval<CHAR, INDEX> &interval_info)
  {

    if (this->map.find(interval_info.parent) == this->map.end())
    {
      this->map[interval_info.parent] = IntervalInfoForMinimalSubstrings<CHAR, INDEX>();
    }
    IntervalInfoForMinimalSubstrings<CHAR, INDEX> &parent_info = this->map[interval_info.parent];

    if (interval_info.interval.i == interval_info.interval.j)
    {
      parent_info.addChar(interval_info.bwtChar, 1, interval_info.charRankOrID, 1);
      //std::cout << id << ": addChar, parent=" << interval_info.parent << "/" << interval_info.bwtChar << "/" << 1 << "/" << interval_info.charRank << std::endl;
      return false;
    }
    else
    {
      INDEX id = interval_info.charRankOrID;
      //std::cout << id << std::endl;
      IntervalInfoForMinimalSubstrings<CHAR, INDEX> &item_info = this->map[id];
      item_info.interval = interval_info.interval;
      bool b = item_info.take(first_occcurrence_map_on_F, this->outputQueue);
      parent_info.add(item_info);
      this->map.erase(id);

      if(interval_info.interval.lcp == 0){
        this->outputQueue.push(interval_info.interval);
      }

      return b;
    }
  }

  bool succ()
  {
    if (this->outputQueue.size() > 0)
    {
      return true;
    }
    else
    {
      bool b = false;
      while (!this->_iterator.isEnded())
      {
        //this->print();
        SpecializedLCPInterval<CHAR, INDEX> sintv = *this->_iterator;

#ifdef DEBUG_PRINT
        std::cout << sintv.to_string() << std::endl;

        std::cout << "read: "
                  << "id: " << this->_iterator.get_current_i() << sintv.to_string() << std::endl;
#endif
        b = this->add(sintv);
        ++this->_iterator;
        if (b)
          break;
      }
      return b;
    }
  }
  void print()
  {
    for (auto &it : this->map)
    {
      it.second.print(it.first);
    }
  }

public:
  MinimalSubstringIterator() = default;
  MinimalSubstringIterator(std::vector<CHAR> &__bwt, PostorderSSTIterator<CHAR, INDEX, VEC> &__iterator) : _iterator(__iterator)
  {
    this->first_occcurrence_map_on_F = constructCMap(__bwt);
    bool b = this->succ();
    if (!b)
      throw std::logic_error("error");
    this->_currenct_lcp_interval = this->outputQueue.front();
    this->outputQueue.pop();
  }
  MinimalSubstringIterator &operator++()
  {
    bool b = this->succ();
    ++counter_i;
    if (b)
    {
      this->_currenct_lcp_interval = this->outputQueue.front();
      this->outputQueue.pop();
    }
    else
    {
      this->_currenct_lcp_interval = LCPInterval<INDEX>::create_end_marker();
    }
    return *this;
  }

  LCPInterval<INDEX> operator*()
  {
    return this->_currenct_lcp_interval;
  }

  bool isEnded()
  {
    return this->_currenct_lcp_interval.is_special_marker();
  }
  INDEX get_st_counter()
  {
    return this->_iterator.get_current_i();
  }
  INDEX get_counter()
  {
    return this->counter_i;
  }
  /* 
  static std::vector<LCPInterval<INDEX>> constructSortedMinimalSubstringsWithoutSpecialMarker(std::vector<CHAR> &bwt, VEC &sa, VEC &lcpArray)
  {
    stool::esaxx::PostorderSSTIterator<CHAR, INDEX> sst = stool::esaxx::PostorderSSTIterator<CHAR, INDEX>::constructIterator(bwt, sa, lcpArray);
    stool::esaxx::MinimalSubstringIterator<CHAR, INDEX> msi(bwt, sst);
    std::vector<LCPInterval<INDEX>> r;

    INDEX maxNodeCount = bwt.size() * 2;
    while (!msi.isEnded())
    {
      stool::LCPInterval<INDEX> p = *msi;

      if (msi.get_counter() % 100000 == 0)
        std::cout << "\r"
                  << "constructing Minimal Substrings : [" << msi.get_st_counter() << "/" << maxNodeCount << "]" << std::flush;
      if (!(p.i == 0 && p.i == p.j))
      {
        r.push_back(stool::LCPInterval<INDEX>(p.i - 1, p.j - 1, p.lcp));
      }
      ++msi;
    }
    std::cout << "[END]" << std::endl;
    std::sort(
        r.begin(),
        r.end(),
        LCPIntervalComp<INDEX>());
    return r;
  }
  */
  static std::vector<LCPInterval<INDEX>> constructSortedMinimalSubstrings(std::vector<CHAR> &bwt, VEC &sa, VEC &lcpArray)
  {
    assert(bwt.size() == sa.size());
    assert(bwt.size() == lcpArray.size());
    stool::esaxx::PostorderSSTIterator<CHAR, INDEX> sst = stool::esaxx::PostorderSSTIterator<CHAR, INDEX>::constructIterator(bwt, sa, lcpArray);
    stool::esaxx::MinimalSubstringIterator<CHAR, INDEX> msi(bwt, sst);
    std::vector<LCPInterval<INDEX>> r;
    INDEX maxNodeCount = bwt.size() * 2;
    while (!msi.isEnded())
    {
      stool::LCPInterval<INDEX> p = *msi;
      r.push_back(stool::LCPInterval<INDEX>(p.i, p.j, p.lcp));


      if (msi.get_counter() % 100000 == 0)
        std::cout << "\r"
                  << "constructing Minimal Substrings : [" << msi.get_st_counter() << "/" << maxNodeCount << "]" << std::flush;

#ifdef DEBUG
      for (uint64_t x = p.i; x <= p.j; x++)
      {
        if(p.lcp != 0){
        INDEX occ = sa[x];
        if (occ + p.lcp - 1 >= bwt.size())
        {
          std::cout << occ << std::endl;
          std::cout << p.to_string() << std::endl;
        }
        assert(occ + p.lcp - 1 < bwt.size());
        }
      }
#endif

      ++msi;
    }
    std::sort(
        r.begin(),
        r.end(),
        LCPIntervalComp<INDEX>());

    return r;
  }
};
} // namespace esaxx
} // namespace stool