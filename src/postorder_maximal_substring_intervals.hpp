#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "stool/src/cmdline.h"
#include <exception>
#include "stool/src/io.hpp"
#include "stool/src/print.hpp"
#include "main/common.hpp"

#include "stool/src/sa_bwt_lcp.hpp"

#include "minimal_substrings/postorder_special_suffix_tree.hpp"

namespace stool
{
    namespace esaxx{

template <typename CHAR, typename INDEX>
struct MaximalSubstringChildInfo
{
    INDEX i;
    CHAR bwtChar;
    bool hasSingleBWTCharacter;

    MaximalSubstringChildInfo()
    {
    }
    MaximalSubstringChildInfo(INDEX _i, CHAR _bwtChar, bool _hasSingleBWTCharacter) : i(_i), bwtChar(_bwtChar), hasSingleBWTCharacter(_hasSingleBWTCharacter)
    {
    }
};

template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename SA = std::vector<uint64_t>, typename LCP = std::vector<uint64_t>, typename BWT = std::vector<CHAR>>
class PostorderMaximalSubstringIntervals
{

    using STI = esaxx::PostorderSuffixTreeIntervals<INDEX, SA, LCP>;
    //using PSSTI = esaxx::PostorderSpecialSuffixTreeIntervals<CHAR, INDEX, SA, LCP, BWT>;

    template <typename SA_ITERATOR = std::vector<uint64_t>, typename LCP_ITERATOR = std::vector<uint64_t>, typename BWT_ITERATOR = typename std::vector<CHAR>::const_iterator>
    class iterator
    {
        using STI_ITERATOR = typename STI::template iterator<SA_ITERATOR, LCP_ITERATOR>;
        BWT_ITERATOR _bwt_iterator;
        STI_ITERATOR _st_iterator;
        INDEX bwt_index = 0;
        INDEX index;
        //stool::LCPInterval<INDEX> currentMSInterval;
        std::stack<MaximalSubstringChildInfo<CHAR, INDEX>> childStack;

        bool compute_next_special_interval()
        {
            stool::LCPInterval<INDEX> x = *_st_iterator;
            MaximalSubstringChildInfo<CHAR, INDEX> x_info(x.i, 0, true);

            INDEX childCount = 0;

            while (childStack.size() > 0)
            {

                MaximalSubstringChildInfo<CHAR, INDEX> top = childStack.top();
                //LCPInterval<INDEX> &childInterval = top.interval;

                if (x.i <= top.i && top.i <= x.j)
                {
                    if (childCount > 0)
                    {
                        if (top.hasSingleBWTCharacter)
                        {
                            if (top.bwtChar != x_info.bwtChar)
                            {
                                x_info.hasSingleBWTCharacter = false;
                            }
                        }
                        else
                        {
                            x_info.hasSingleBWTCharacter = false;
                        }
                    }
                    else
                    {
                        if (top.hasSingleBWTCharacter)
                        {
                            x_info.hasSingleBWTCharacter = true;
                            x_info.bwtChar = top.bwtChar;
                        }
                        else
                        {
                            x_info.hasSingleBWTCharacter = false;
                        }
                    }
                    childStack.pop();
                    childCount++;
                }
                else
                {
                    break;
                }
            }
            if (x.i == x.j)
            {
                while (bwt_index < x.i)
                {
                    ++_bwt_iterator;
                    ++bwt_index;
                }
                x_info.bwtChar = *_bwt_iterator;
            }
            childStack.push(x_info);

            return !x_info.hasSingleBWTCharacter || x.lcp == _st_iterator.get_text_size();
        }

    public:
        iterator() = default;
        iterator(BWT_ITERATOR &__bwt_iterator, STI_ITERATOR &__st_iterator) : _bwt_iterator(std::move(__bwt_iterator)), _st_iterator(std::move(__st_iterator))
        {
            if (_st_iterator.isEnded())
            {
                this->index = std::numeric_limits<INDEX>::max();
            }
            else
            {
                this->succ(true);
            }
        }

    private:
        void succ(bool isFirst)
        {
            if (isFirst)
            {
                index = 0;
            }
            else
            {
                ++_st_iterator;
                ++index;
            }
            while (!_st_iterator.isEnded())
            {
                bool b = this->compute_next_special_interval();
                if (b)
                {
                    break;
                }
                else
                {
                    ++_st_iterator;
                }
            }
            if (_st_iterator.isEnded())
            {
                this->index = std::numeric_limits<INDEX>::max();
            }
        }

    public:
        iterator &operator++()
        {
            this->succ(false);
            return *this;
        }
        stool::LCPInterval<INDEX> operator*()
        {
            return *_st_iterator;
        }
        bool operator!=(const iterator &rhs) const
        {
            return index != rhs.index;
        }
        bool isEnded()
        {
            return _st_iterator.isEnded();
        }
    };

private:
    STI *_sti;
    const BWT *_bwt;
    bool deleteFlag = false;

public:
    void construct(const SA *__sa, const LCP *__lcp, const BWT *__bwt)
    {
        this->_sti = new STI();
        this->_sti->construct(__sa, __lcp);
        this->_bwt = __bwt;
    }
    const STI *get_sti_pointer() const
    {
        return this->_sti;
    }
    const BWT *get_bwt_pointer() const
    {
        return this->_bwt;
    }
    const SA *get_sa_pointer() const
    {
        return this->_sti->get_sa_pointer();
    }
    const LCP *get_lcp_pointer() const
    {
        return this->_sti->get_lcp_pointer();
    }
    auto begin() const -> iterator<decltype(this->get_sa_pointer()->begin()), decltype(this->get_lcp_pointer()->begin()), decltype(this->get_bwt_pointer()->begin())>
    {
        using BWT_IT = decltype(this->get_bwt_pointer()->begin());
        using SA_IT = decltype(this->get_sa_pointer()->begin());
        using LCP_IT = decltype(this->get_lcp_pointer()->begin());

        auto sti_beg = this->get_sti_pointer()->begin();
        auto bwt_beg = this->get_bwt_pointer()->begin();

        auto it = iterator<SA_IT, LCP_IT, BWT_IT>(bwt_beg, sti_beg);
        return it;
    }

    auto end() const -> iterator<decltype(this->get_sa_pointer()->begin()), decltype(this->get_lcp_pointer()->begin()), decltype(this->get_bwt_pointer()->begin())>
    {
        using BWT_IT = decltype(this->get_bwt_pointer()->begin());
        using SA_IT = decltype(this->get_sa_pointer()->begin());
        using LCP_IT = decltype(this->get_lcp_pointer()->begin());

        auto sti_end = this->get_sti_pointer()->end();
        auto bwt_beg = this->get_bwt_pointer()->begin();

        auto it = iterator<SA_IT, LCP_IT, BWT_IT>(bwt_beg, sti_end);
        return it;
    }

    //template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename SA = std::vector<INDEX>, typename LCP = std::vector<INDEX>>
    static std::vector<stool::LCPInterval<INDEX>> compute_maximal_substrings(const SA &sa, const LCP &lcpArray, const BWT &bwt)
    {
        std::vector<stool::LCPInterval<INDEX>> r;
        PostorderMaximalSubstringIntervals<CHAR, INDEX, SA, LCP, BWT > pmsi;
        pmsi.construct(&sa, &lcpArray, &bwt);
        for (auto it : pmsi)
        {
            r.push_back(it);
        }
        return r;
    }
};

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
    }
} // namespace stool