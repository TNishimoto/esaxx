#pragma once
#include "minimal_substring_candidates.hpp"
#include "stool/include/print.hpp"
#include "../main/common.hpp"

namespace stool
{
  namespace esaxx
  {

    template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename SA = std::vector<uint64_t>, typename LCP = std::vector<uint64_t>, typename BWT = std::vector<CHAR>>
    class MinimalSubstringIntervals
    {

      using STI = PostorderSuffixTreeIntervals<INDEX, SA, LCP>;
      using PSSTI = PostorderSpecialSuffixTreeIntervals<CHAR, INDEX, SA, LCP, BWT>;

    public:
      template <typename SA_ITERATOR = std::vector<uint64_t>, typename LCP_ITERATOR = std::vector<uint64_t>, typename BWT_ITERATOR = typename std::vector<CHAR>::const_iterator>
      class iterator
      {
        using PSSTI_ITERATOR = typename PSSTI::template iterator<SA_ITERATOR, LCP_ITERATOR, BWT_ITERATOR>;

        PSSTI_ITERATOR _iterator;
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

          bool b = false;

          if (interval_info.interval.i == interval_info.interval.j && interval_info.interval.lcp != 0)
          {
            parent_info.addChar(interval_info.bwtChar, 1, interval_info.charRankOrID, 1);
            // std::cout << id << ": addChar, parent=" << interval_info.parent << "/" << interval_info.bwtChar << "/" << 1 << "/" << interval_info.charRank << std::endl;
          }
          else
          {
            INDEX id = interval_info.charRankOrID;
            // std::cout << id << std::endl;
            IntervalInfoForMinimalSubstrings<CHAR, INDEX> &item_info = this->map[id];
            item_info.interval = interval_info.interval;
            b = item_info.take(first_occcurrence_map_on_F, this->outputQueue);
            parent_info.add(item_info);
            this->map.erase(id);
          }

          if (interval_info.interval.lcp == 0)
          {
            this->outputQueue.push(interval_info.interval);
            b = true;
          }
          return b;
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
              // this->print();
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
        iterator() = default;
        iterator(const BWT &__bwt, PSSTI_ITERATOR &__iterator) : _iterator(__iterator)
        {
          this->first_occcurrence_map_on_F = constructCMap(__bwt);
          bool b = this->succ();

          if (b)
          {
            this->_currenct_lcp_interval = this->outputQueue.front();
            this->outputQueue.pop();
          }
          else
          {
            this->_currenct_lcp_interval = LCPInterval<INDEX>::create_end_marker();
          }
          /*
          if (!b){
            std::cout << this->outputQueue.size() << std::endl;
            throw std::logic_error("MinimalSubstringIterator Initialization error");
          }
          */
        }
        iterator &operator++()
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

        bool operator!=(const iterator &rhs)
        {
          // return false;
          return this->_iterator != rhs._iterator;
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

        static std::vector<INDEX> constructMSIntervalParents(const std::vector<LCPInterval<INDEX>> &intervals)
        {
          std::vector<INDEX> outputParents;
          std::stack<INDEX> stack;
          if (intervals.size() > 100000)
            std::cout << "Constructing minimal substring tree" << std::flush;
          stool::Counter counter;
          outputParents.resize(intervals.size(), std::numeric_limits<INDEX>::max());
          for (INDEX i = 0; i < intervals.size(); i++)
          {
            if (intervals.size() > 100000)
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
          if (intervals.size() > 100000)
            std::cout << "[END]" << std::endl;
          return outputParents;
        }
      };

    private:
      PSSTI *_pssti;
      bool deleteFlag = false;

    public:
      /*
    void set(BWT &__bwt, STI &__sti)
    {
      this->_bwt = &__bwt;
      this->_sti = &__sti;
      deleteFlag = false;
    }
    */

      void construct(SA *__sa, LCP *__lcp, BWT *__bwt)
      {
        this->_pssti = new PSSTI();
        this->_pssti->construct(__sa, __lcp, __bwt);
        // this->_bwt = __bwt;
        // this->sti = new STI();
        //   this->sti->set(__sa, __lcp);
      }
      /*
      const BWT* get_bwt_pointer() const {
        return this->_bwt;
      }
      */
      const PSSTI *get_pssti_pointer() const
      {
        return this->_pssti;
      }
      const BWT *get_bwt_pointer() const
      {
        return this->_pssti->get_bwt_pointer();
      }
      const SA *get_sa_pointer() const
      {
        return this->_pssti->get_sa_pointer();
      }
      const LCP *get_lcp_pointer() const
      {
        return this->_pssti->get_lcp_pointer();
      }

      auto begin() const -> iterator<decltype(this->get_sa_pointer()->begin()), decltype(this->get_lcp_pointer()->begin()), decltype(this->get_bwt_pointer()->begin())>
      {
        using BWT_IT = decltype(this->get_bwt_pointer()->begin());
        using SA_IT = decltype(this->get_sa_pointer()->begin());
        using LCP_IT = decltype(this->get_lcp_pointer()->begin());

        auto bwt = this->get_bwt_pointer();
        auto pssti_beg = this->get_pssti_pointer()->begin();

        auto it = iterator<SA_IT, LCP_IT, BWT_IT>(*bwt, pssti_beg);
        return it;
      }

      auto end() const -> iterator<decltype(this->get_sa_pointer()->begin()), decltype(this->get_lcp_pointer()->begin()), decltype(this->get_bwt_pointer()->begin())>
      {
        using BWT_IT = decltype(this->get_bwt_pointer()->begin());
        using SA_IT = decltype(this->get_sa_pointer()->begin());
        using LCP_IT = decltype(this->get_lcp_pointer()->begin());

        auto bwt = this->get_bwt_pointer();
        auto pssti_end = this->get_pssti_pointer()->end();

        auto it = iterator<SA_IT, LCP_IT, BWT_IT>(*bwt, pssti_end);
        return it;
      }
    };

    template <typename INDEX = uint64_t>
    void getKMinimalSubstrings(std::vector<LCPInterval<INDEX>> &intervals, uint64_t limitLength)
    {
      std::vector<LCPInterval<INDEX>> r;
      for (uint64_t i = 0; i < intervals.size(); i++)
      {
        if (intervals[i].lcp <= limitLength)
        {
          r.push_back(intervals[i]);
        }
      }
      intervals.swap(r);
    }

    template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename SA = std::vector<INDEX>, typename LCP = std::vector<INDEX>>
    std::vector<stool::LCPInterval<INDEX>> compute_minimal_substrings_using_bwt(std::vector<CHAR> &bwt, SA &sa, LCP &lcpArray)
    {
      std::vector<LCPInterval<INDEX>> r;

      stool::esaxx::MinimalSubstringIntervals<CHAR, INDEX, SA, LCP, std::vector<CHAR>> msi;
      msi.construct(&sa, &lcpArray, &bwt);

      auto it = msi.begin();

      while (!it.isEnded())
      {
        r.push_back(*it);
        ++it;
      }
      return r;
    }

    template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename SA = std::vector<INDEX>, typename LCP = std::vector<INDEX>>
    std::vector<stool::LCPInterval<INDEX>> compute_minimal_substrings(std::vector<CHAR> &text, SA &sa, LCP &lcpArray, bool needCheckText)
    {
      if (needCheckText)
      {
        stool::esaxx::check_dollar_text(text);
      }
      std::vector<CHAR> bwt = stool::esaxx::constructBWT<CHAR, INDEX, SA>(text, sa);
      return compute_minimal_substrings_using_bwt(bwt, sa, lcpArray);
    }
    // template <typename SA = std::vector<INDEX>, typename LCP = std::vector<INDEX> >
    template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename SA = std::vector<INDEX>, typename LCP = std::vector<INDEX>>
    std::vector<LCPInterval<INDEX>> constructSortedMinimalSubstrings(std::vector<CHAR> &bwt, SA &sa, LCP &lcpArray)
    {
      std::vector<LCPInterval<INDEX>> r = compute_minimal_substrings_using_bwt(bwt, sa, lcpArray);

      std::sort(
          r.begin(),
          r.end(),
          stool::LCPIntervalPreorderComp<INDEX>());
      return r;
    }
  } // namespace esaxx
} // namespace stool