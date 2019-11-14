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

template <typename INDEXTYPE, typename POSTORDER_SST_ITERATOR>
class PostorderMaximalSubstringIntervalIterator
{
    POSTORDER_SST_ITERATOR st;
    INDEXTYPE index;
    stool::LCPInterval<INDEXTYPE> currentMSInterval;

public:
    PostorderMaximalSubstringIntervalIterator() = default;
    PostorderMaximalSubstringIntervalIterator(POSTORDER_SST_ITERATOR &_st) : st(std::move(_st))
    {
        if (st.isEnded())
        {
            this->index = std::numeric_limits<INDEXTYPE>::max();
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
    PostorderMaximalSubstringIntervalIterator &operator++()
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
            this->index = std::numeric_limits<INDEXTYPE>::max();
        }
        return *this;
    }
    stool::LCPInterval<INDEXTYPE> operator*()
    {
        return (*st).interval;
    }
    bool operator!=(const PostorderMaximalSubstringIntervalIterator &rhs) const
    {
        return index != rhs.index;
    }
    bool isEnded(){
        return st.isEnded();
    }
};

template <typename CHAR = char, typename INDEX = uint64_t, typename SA = std::vector<INDEX>, typename LCP = std::vector<INDEX>>
std::vector<stool::LCPInterval<INDEX>> compute_preorder_maximal_substrings(const std::vector<CHAR> &text, const SA &sa, const LCP &lcpArray)
{  
  //stool::Printer::print(text);
  //std::vector<INDEX> lcpArray = stool::constructLCP<CHAR, INDEX>(text, sa);
  //std::vector<CHAR> bwt = stool::constructBWT<CHAR, INDEX>(text, sa);
  std::vector<CHAR> bwt = stool::esaxx::constructBWT<CHAR, INDEX, SA>(text, sa);

  using PST = stool::esaxx::PostorderSuffixTreeIntervals<INDEX, SA, LCP>;
  using PSTIT = typename PST::template iterator<decltype(sa.begin()), decltype(lcpArray.begin()) >;
  using BWTIT = typename std::vector<CHAR>::const_iterator;
  using SST = stool::esaxx::PostorderSSTIterator<CHAR, INDEX, PSTIT, BWTIT>;

    using SSTL = stool::esaxx::PostorderSSTIterator<CHAR, INDEX, typename esaxx::PostorderSuffixTreeIntervals<INDEX, SA, LCP>::template iterator<decltype(sa.begin()), decltype(lcpArray.begin())>  >;
    SSTL sst = stool::esaxx::PostorderSSTIterator<CHAR, INDEX>::template constructIterator<SA, LCP>(bwt, sa, lcpArray);
    /*
  PST pst;
  pst.set(sa, lcpArray);
  PSTIT pstbeg = pst.begin();
  BWTIT bwtI = bwt.cbegin();
  SST sst(bwtI, pstbeg, true);
  */
  PostorderMaximalSubstringIntervalIterator<INDEX, SST> msi(sst);
  
  std::vector<LCPInterval<INDEX>> r;
  
  while(!msi.isEnded()){
      r.push_back(*msi);
      ++msi;
  }

    std::sort(
        r.begin(),
        r.end(),
        stool::LCPIntervalPreorderComp<INDEX>());
        
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