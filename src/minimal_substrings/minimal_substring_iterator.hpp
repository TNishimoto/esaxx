#pragma once
#include "minimal_substring_candidates.hpp"
#include "stool/src/print.hpp"
namespace stool
{
namespace esaxx
{

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
  MinimalSubstringIterator(const std::vector<CHAR> &__bwt, PostorderSSTIterator<CHAR, INDEX, VEC> &__iterator) : _iterator(__iterator)
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
  static std::vector<LCPInterval<INDEX>> constructSortedMinimalSubstrings(const std::vector<CHAR> &bwt,const  VEC &sa,const  VEC &lcpArray)
  {
    assert(bwt.size() == sa.size());
    assert(bwt.size() == lcpArray.size());
    stool::esaxx::PostorderSSTIterator<CHAR, INDEX> sst = stool::esaxx::PostorderSSTIterator<CHAR, INDEX>::constructIterator(bwt, sa, lcpArray);
    stool::esaxx::MinimalSubstringIterator<CHAR, INDEX> msi(bwt, sst);
    std::vector<LCPInterval<INDEX>> r;
    if(bwt.size() > 100000)std::cout << "Conputing minimal substrings" << std::flush;
    stool::Counter counter;
    while (!msi.isEnded())
    {
      stool::LCPInterval<INDEX> p = *msi;
      r.push_back(stool::LCPInterval<INDEX>(p.i, p.j, p.lcp));

      if(bwt.size() > 100000)counter.increment();

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
    if(bwt.size() > 100000)std::cout << "[END]"<< std::endl;



    std::sort(
        r.begin(),
        r.end(),
        LCPIntervalComp<INDEX>());

    return r;
  }
  static void getKMinimalSubstrings(std::vector<LCPInterval<INDEX>> &intervals, uint64_t limitLength){
      std::vector<LCPInterval<INDEX>> r;
      for(uint64_t i=0;i<intervals.size();i++){
        if(intervals[i].lcp <= limitLength){
        r.push_back(intervals[i]);
        }
      }
      intervals.swap(r);
  }

  static std::vector<INDEX> constructMSIntervalParents(const std::vector<LCPInterval<INDEX>> &intervals)
  {
    std::vector<INDEX> outputParents;
    std::stack<INDEX> stack;
        std::cout << "Constructing minimal substring tree" << std::flush;
    stool::Counter counter;
    outputParents.resize(intervals.size(), std::numeric_limits<INDEX>::max());
    for (INDEX i = 0; i < intervals.size(); i++)
    {
      counter.increment();
      const LCPInterval<INDEX> &interval = intervals[i];

      while (stack.size() > 0)
      {
        const LCPInterval<INDEX> &parentInterval = intervals[stack.top()];

        if (parentInterval.i <= interval.i && interval.j <= parentInterval.j)
        {
          break;
        }
        else
        {
          stack.pop();
        }
      }
      if (stack.size() > 0)
      {
        outputParents[i] = stack.top();
      }
      stack.push(i);
    }
    std::cout << "[END]" << std::endl;
    return outputParents;
  }
};
} // namespace esaxx
} // namespace stool