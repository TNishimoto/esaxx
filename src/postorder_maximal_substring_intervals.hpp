#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include "stool/src/cmdline.h"
#include <exception>
#include "stool/src/io.hpp"
#include "stool/src/print.hpp"
#include "main/common.hpp"

#include "stool/src/sa_bwt_lcp.hpp"

//#include "minimal_substrings/postorder_special_suffix_tree.hpp"
#include "postorder_lcp_intervals.hpp"
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

template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename LCP = std::vector<uint64_t>, typename BWT = std::vector<CHAR>>
class PostorderMaximalSubstringIntervals
{

    using STI = esaxx::PostorderLCPIntervals<INDEX, LCP>;
    using LCP_ITERATOR = typename LCP::const_iterator;
    using BWT_ITERATOR = typename BWT::const_iterator;
    //using PSSTI = esaxx::PostorderSpecialSuffixTreeIntervals<CHAR, INDEX, SA, LCP, BWT>;
        using STI_ITERATOR = typename STI::iterator;

    //template <typename SA_ITERATOR = std::vector<uint64_t>, typename LCP_ITERATOR = std::vector<uint64_t>, typename BWT_ITERATOR = typename std::vector<CHAR>::const_iterator>
    class iterator
    {
        BWT_ITERATOR _bwt_iterator;
        STI_ITERATOR _st_iterator;
        INDEX bwt_index = 0;
        INDEX index;
        //stool::LCPInterval<INDEX> currentMSInterval;
        std::stack<MaximalSubstringChildInfo<CHAR, INDEX>> childStack;

        bool compute_next_special_interval()
        {
            stool::LCPInterval<INDEX> x = *_st_iterator;
            //std::cout << x.to_string() << std::endl;
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
            if(x.i == x.j && x_info.bwtChar == 0){
                x.lcp = _st_iterator.get_text_size();
            }

            return !x_info.hasSingleBWTCharacter || x.lcp == _st_iterator.get_text_size();
            //;
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
            auto p = *_st_iterator;
            if(p.i == p.j && p.lcp == std::numeric_limits<INDEX>::max()-1){
                p.lcp = _st_iterator.get_text_size();
            }
            return p;
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
    void construct(const LCP *__lcp, const BWT *__bwt)
    {
        this->_sti = new STI();
        this->_sti->construct(__lcp);
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
    const LCP *get_lcp_pointer() const
    {
        return this->_sti->get_lcp_pointer();
    }
    iterator begin() const
    {
        //using BWT_IT = decltype(this->get_bwt_pointer()->begin());
        //using SA_IT = decltype(this->get_sa_pointer()->begin());
        //using LCP_IT = decltype(this->get_lcp_pointer()->begin());

        STI_ITERATOR sti_beg = this->get_sti_pointer()->begin();
        BWT_ITERATOR bwt_beg = this->get_bwt_pointer()->begin();

        auto it = iterator(bwt_beg, sti_beg);
        return it;
    }

    iterator end() const
    {
        //using BWT_IT = decltype(this->get_bwt_pointer()->begin());
        //using SA_IT = decltype(this->get_sa_pointer()->begin());
        //using LCP_IT = decltype(this->get_lcp_pointer()->begin());

        STI_ITERATOR sti_end = this->get_sti_pointer()->end();
        BWT_ITERATOR bwt_beg = this->get_bwt_pointer()->begin();

        auto it = iterator(bwt_beg, sti_end);
        return it;
    }

    //template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename SA = std::vector<INDEX>, typename LCP = std::vector<INDEX>>
    static std::vector<stool::LCPInterval<INDEX>> compute_maximal_substrings(const LCP &lcpArray, const BWT &bwt)
    {
        std::vector<stool::LCPInterval<INDEX>> r;
        PostorderMaximalSubstringIntervals<CHAR, INDEX, LCP, BWT > pmsi;
        pmsi.construct(&lcpArray, &bwt);
        for (auto it : pmsi)
        {
            r.push_back(it);
        }
        return r;
    }
};

    }
} // namespace stool