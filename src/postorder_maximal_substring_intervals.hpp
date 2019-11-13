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

#include "stool/src/sa_bwt_lcp.hpp"

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

template <typename INDEXTYPE, typename POSTORDER_ST_ITERATOR, typename DISTINCT_CHARACTER_CHECKER>
class PostorderMaximalSubstringIntervalIterator
{
    POSTORDER_ST_ITERATOR st;
    const DISTINCT_CHARACTER_CHECKER &distinct_character_checker;
    INDEXTYPE index;
    stool::LCPInterval<INDEXTYPE> currentMSInterval;

public:
    PostorderMaximalSubstringIntervalIterator() = default;
    PostorderMaximalSubstringIntervalIterator(POSTORDER_ST_ITERATOR &_st, const DISTINCT_CHARACTER_CHECKER &_distinct_character_checker) : st(std::move(_st)), distinct_character_checker(_distinct_character_checker)
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
        auto interval = *st;
        return distinct_character_checker.contains(interval.i, interval.j) || interval.lcp == st.get_text_size();
    }

public:
    PostorderMaximalSubstringIntervalIterator &operator++()
    {
        ++st;
        while (!st.isEnded())
        {
            auto interval = *st;
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
        return *st;
    }
    bool operator!=(const PostorderMaximalSubstringIntervalIterator &rhs) const
    {
        return index != rhs.index;
    }
};


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

    /*
    PostorderMSIterator<INDEXTYPE> end() const
    {
        auto it = PostorderMSIterator<INDEXTYPE>(this->L, this->R, this->D, std::numeric_limits<INDEXTYPE>::max());
        return it;
    }
    */
    INDEX size() const
    {
        return 0;
    }
};

} // namespace stool