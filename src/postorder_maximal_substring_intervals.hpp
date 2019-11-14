#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "stool/src/cmdline.h"
#include "esa.hxx"
#include <exception>
#include "stool/src/io.hpp"
#include "stool/src/print.hpp"
#include "main/common.hpp"

#include "stool/src/sa_bwt_lcp.hpp"

#include "minimal_substrings/postorder_special_suffix_tree.hpp"

namespace stool
{

class DistinctCharacterCheckerOnInterval
{
    std::vector<uint64_t> vec;

public:
    bool contains(uint64_t left, uint64_t right) const
    {
        return vec[right] - vec[left] != 0;
    }

    template <typename CHAR, typename TEXT, typename SA>
    void construct(TEXT &text, SA &sa)
    {
        vec.resize(text.size(), 0);
        for (uint64_t i = 1; i < text.size(); i++)
        {
            uint64_t p = sa[i];
            uint64_t prev_p = sa[i - 1];

            CHAR prev_c = prev_p == 0 ? text[text.size() - 1] : text[prev_p - 1];
            CHAR c = p == 0 ? text[text.size() - 1] : text[p - 1];

            if (c != prev_c)
            {
                vec[i] = vec[i - 1] + 1;
            }
            else
            {
                vec[i] = vec[i - 1];
            }
        }
    }
};

template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename SA = std::vector<uint64_t>, typename LCP = std::vector<uint64_t>, typename BWT = std::vector<CHAR>>
class PostorderMaximalSubstringIntervals{

using STI = esaxx::PostorderSuffixTreeIntervals<INDEX, SA, LCP>;
using PSSTI = esaxx::PostorderSpecialSuffixTreeIntervals<CHAR, INDEX, SA, LCP, BWT>;

template <typename SA_ITERATOR = std::vector<uint64_t>, typename LCP_ITERATOR = std::vector<uint64_t>, typename BWT_ITERATOR = typename std::vector<CHAR>::const_iterator>
class iterator
{
  using PSSTI_ITERATOR = typename PSSTI::template iterator<SA_ITERATOR, LCP_ITERATOR, BWT_ITERATOR>;
    PSSTI_ITERATOR st;
    INDEX index;
    stool::LCPInterval<INDEX> currentMSInterval;

public:
    iterator() = default;
    iterator(PSSTI_ITERATOR &_st) : st(std::move(_st))
    {
        if (st.isEnded())
        {
            this->index = std::numeric_limits<INDEX>::max();
        }
        else
        {
            if(this->isMSInterval()){
                this->index = 1;
            }else{
                this->index = 0;
                ++(*this);
            }

            /*
            auto interval = *st;
                if(distinct_character_checker.contains(interval.i, interval.j)){
                    index = 0;
                }else{
                    ++this;
                    //++st;
                }
            */
        }
    }

private:
    bool isMSInterval()
    {
        auto sinterval = *st;
        return !sinterval.hasSingleBWTCharacter || sinterval.interval.lcp == st.get_text_size();
    }

public:
    iterator &operator++()
    {
        ++st;
        while (!st.isEnded())
        {
            //auto interval = *st;
            if(this->isMSInterval()){
                index++;
                break;
            }else{
                ++st;
            }
        }
        if (st.isEnded())
        {
            this->index = std::numeric_limits<INDEX>::max();
        }
        return *this;
    }
    stool::LCPInterval<INDEX> operator*()
    {
        return (*st).interval;
    }
    bool operator!=(const iterator &rhs) const
    {
        return index != rhs.index;
    }
    bool isEnded(){
        return st.isEnded();
    }
};


private:
  PSSTI *_pssti;
  bool deleteFlag = false;
  public:


  void construct(const SA* __sa,const LCP* __lcp,const BWT* __bwt){
      this->_pssti = new PSSTI();
      this->_pssti->construct(__sa, __lcp, __bwt);
  }
  const PSSTI* get_pssti_pointer() const {
    return this->_pssti;
  }
  const BWT* get_bwt_pointer() const {
    return this->_pssti->get_bwt_pointer();
  }
  const SA* get_sa_pointer() const{
      return this->_pssti->get_sa_pointer();
  }
  const LCP* get_lcp_pointer() const{
      return this->_pssti->get_lcp_pointer();
  }
auto begin() const -> iterator< decltype(this->get_sa_pointer()->begin() ), decltype(this->get_lcp_pointer()->begin() ) ,  decltype(this->get_bwt_pointer()->begin() )>
  {
    using BWT_IT = decltype(this->get_bwt_pointer()->begin() );
    using SA_IT = decltype(this->get_sa_pointer()->begin() );
    using LCP_IT = decltype(this->get_lcp_pointer()->begin() );

    auto pssti_beg = this->get_pssti_pointer()->begin();


    auto it = iterator<SA_IT, LCP_IT, BWT_IT>(pssti_beg);
    return it;
  }
  
  auto end() const -> iterator< decltype(this->get_sa_pointer()->begin() ), decltype(this->get_lcp_pointer()->begin() ) ,  decltype(this->get_bwt_pointer()->begin() )>
  {    
    using BWT_IT = decltype(this->get_bwt_pointer()->begin() );
    using SA_IT = decltype(this->get_sa_pointer()->begin() );
    using LCP_IT = decltype(this->get_lcp_pointer()->begin() );

    auto pssti_end = this->get_pssti_pointer()->end();


    auto it = iterator<SA_IT, LCP_IT, BWT_IT>(pssti_end);
    return it;
  }
  
};




template <typename CHAR = char, typename INDEX = uint64_t, typename SA = std::vector<INDEX>, typename LCP = std::vector<INDEX>>
std::vector<stool::LCPInterval<INDEX>> compute_preorder_maximal_substrings(const std::vector<CHAR> &text, const SA &sa, const LCP &lcpArray)
{  
  //stool::Printer::print(text);
  //std::vector<INDEX> lcpArray = stool::constructLCP<CHAR, INDEX>(text, sa);
  //std::vector<CHAR> bwt = stool::constructBWT<CHAR, INDEX>(text, sa);
  std::vector<LCPInterval<INDEX>> r;
  
  std::vector<CHAR> bwt = stool::esaxx::constructBWT<CHAR, INDEX, SA>(text, sa);
  PostorderMaximalSubstringIntervals<CHAR, INDEX, SA, LCP, std::vector<CHAR>> pmsi;
    pmsi.construct(&sa, &lcpArray, &bwt);
    for(auto it : pmsi){
        r.push_back(it);
    }


    std::sort(
        r.begin(),
        r.end(),
        stool::LCPIntervalPreorderComp<INDEX>());

    /*
  using PST = stool::esaxx::PostorderSuffixTreeIntervals<INDEX, SA, LCP>;
  using PSTIT = typename PST::template iterator<decltype(sa.begin()), decltype(lcpArray.begin()) >;
  using BWTIT = typename std::vector<CHAR>::const_iterator;
  using SST = stool::esaxx::PostorderSSTIterator<CHAR, INDEX, PSTIT, BWTIT>;

    using SSTL = stool::esaxx::PostorderSSTIterator<CHAR, INDEX, typename esaxx::PostorderSuffixTreeIntervals<INDEX, SA, LCP>::template iterator<decltype(sa.begin()), decltype(lcpArray.begin())>  >;
    SSTL sst = stool::esaxx::PostorderSSTIterator<CHAR, INDEX>::template constructIterator<SA, LCP>(bwt, sa, lcpArray);
  PostorderMaximalSubstringIntervalIterator<INDEX, SST> msi(sst);
  */
  
  /*
  
  while(!msi.isEnded()){
      r.push_back(*msi);
      ++msi;
  }

        */
  return r;
}

/*
template <typename INDEX, typename POSTORDER_ST, typename DISTINCT_CHARACTER_CHECKER>
class PostorderMaximalSubstringIntervals
{
  const POSTORDER_ST *_ST;
  const DISTINCT_CHARACTER_CHECKER *_BWTChecker;
  bool deleteFlag = false;


public:
    PostorderMaximalSubstringIntervals()
    {
    }

  void set(POSTORDER_ST &&__st, DISTINCT_CHARACTER_CHECKER &&__bwt_checker)
  {
    this->_ST = new POSTORDER_ST(std::move(__st));
    this->_BWTChecker = new DISTINCT_CHARACTER_CHECKER(std::move(__bwt_checker));

    deleteFlag = true;
  }
  void set(POSTORDER_ST &__st, DISTINCT_CHARACTER_CHECKER &__bwt_checker)
  {
    this->_ST = &__st;
    this->_BWTChecker = &__bwt_checker;

    deleteFlag = false;
  }


    auto begin() -> PostorderMaximalSubstringIntervalIterator<INDEX,decltype(this->_ST->begin()), DISTINCT_CHARACTER_CHECKER> const
    {
        auto beg = _ST->begin();
        return PostorderMaximalSubstringIntervalIterator<INDEX, decltype(this->_ST->begin()), DISTINCT_CHARACTER_CHECKER>(beg, *this->_BWTChecker);
    }
    auto end() -> PostorderMaximalSubstringIntervalIterator<INDEX,decltype(this->_ST->begin()), DISTINCT_CHARACTER_CHECKER> const
    {
        auto end = _ST->end();
        return PostorderMaximalSubstringIntervalIterator<INDEX, decltype(this->_ST->end()), DISTINCT_CHARACTER_CHECKER>(end, *this->_BWTChecker);
    }

    
    PostorderMSIterator<INDEXTYPE> end() const
    {
        auto it = PostorderMSIterator<INDEXTYPE>(this->L, this->R, this->D, std::numeric_limits<INDEXTYPE>::max());
        return it;
    }
    
    INDEX size() const
    {
        return 0;
    }
};
*/

} // namespace stool