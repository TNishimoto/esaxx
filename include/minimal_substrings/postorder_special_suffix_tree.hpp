#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "stool/include/light_stool.hpp"


#include <stack>
#include "../postorder_suffix_tree_intervals.hpp"


namespace stool
{

namespace esaxx
{
// This class has information for constructing minimal substrings about a suffix tree node.
// This class stores the interval [i..j] of the node on SA, and the length of the edge between the parent and the node.
template <typename CHAR, typename INDEX>
class SpecializedLCPInterval
{
public:
    // The interval of this node on SA.
    LCPInterval<INDEX> interval;
    // The parent index of this node.
    INDEX parent;
    INDEX charRankOrID;
    // If this node is a leaf, the bwtChar is the character of the bwt on this leaf.
    CHAR bwtChar;
    bool hasSingleBWTCharacter;

    SpecializedLCPInterval()
    {
    }
    SpecializedLCPInterval(LCPInterval<INDEX> _interval, INDEX _parent, CHAR _c, INDEX _charRank, bool _hasSingleBWTCharacter) : interval(_interval), parent(_parent), charRankOrID(_charRank), bwtChar(_c), hasSingleBWTCharacter(_hasSingleBWTCharacter)
    {
    }
    /*
    SpecializedLCPInterval(LCPInterval<INDEX> _interval, INDEX _parent, INDEX _id, bool _hasSingleBWTCharacter) : interval(_interval), parent(_parent), charRankOrID(_id), bwtChar(0), hasSingleBWTCharacter(_hasSingleBWTCharacter)
    {
    }
    */

    static SpecializedLCPInterval<CHAR, INDEX> create_end_marker()
    {
        return SpecializedLCPInterval<CHAR, INDEX>(LCPInterval<INDEX>::create_end_marker(), std::numeric_limits<INDEX>::max(), 0, 0,false);
    }

    std::string to_string()
    {
        return "[" + this->interval.to_string() + "," + std::to_string(this->parent) + ", " + std::string(1, bwtChar) + ", " + std::to_string(this->charRankOrID) + "]";
    }

    bool isEnd()
    {
        return this->interval.is_special_marker() && this->parent == std::numeric_limits<INDEX>::max() && this->bwtChar == 0;
    }
};
template <typename CHAR, typename INDEX>
struct SSTChildIntervalInfo
{
    LCPInterval<INDEX> interval;
    INDEX id;
    INDEX charRank;
    CHAR bwt;
    bool hasSingleBWTCharacters;

    SSTChildIntervalInfo()
    {
    }
    SSTChildIntervalInfo(LCPInterval<INDEX> _interval, INDEX _id, INDEX _charRank, CHAR _bwt, bool _hasSingleBWTCharacters) : interval(_interval), id(_id), charRank(_charRank), bwt(_bwt), hasSingleBWTCharacters(_hasSingleBWTCharacters)
    {
    }
};



template <typename CHAR = uint8_t, typename INDEX = uint64_t, typename SA = std::vector<uint64_t>, typename LCP = std::vector<uint64_t>, typename BWT = std::vector<CHAR>>
class PostorderSpecialSuffixTreeIntervals{
    using STI = PostorderSuffixTreeIntervals<INDEX, SA, LCP>;

public: 
template <typename SA_ITERATOR = std::vector<uint64_t>, typename LCP_ITERATOR = std::vector<uint64_t>, typename BWT_ITERATOR = typename std::vector<CHAR>::const_iterator>
class iterator
{
    using ST_ITERATOR = typename PostorderSuffixTreeIntervals<INDEX, SA, LCP>::template iterator<SA_ITERATOR, LCP_ITERATOR>;
    ST_ITERATOR _iterator;
    BWT_ITERATOR _bwt_iterator;
    //INDEX counter_i = 0;
    INDEX current_i = 0;
    std::unordered_map<CHAR, INDEX> charRankMap;
    std::stack<SSTChildIntervalInfo<CHAR, INDEX>> childStack;
    std::queue<SpecializedLCPInterval<CHAR, INDEX>> outputQueue;
    SpecializedLCPInterval<CHAR, INDEX> _currenct_lcp_interval;    
    INDEX bwt_index = 0;

    //LCPIterator<INDEX, VEC> _lcp_forward_iterator;
    //std::queue<LCPInterval<INDEX>> parentQueue;

    //int64_t n = 0;
    bool compute_next_special_interval()
    {
        LCPInterval<INDEX> x = *_iterator;
        INDEX id = this->_iterator.get_current_i();

        bool x_hasSingleBWTCharacter = true;
        CHAR x_BWTChar = 0;
            bool x_hasAtLeastOneChild = false;


        while (childStack.size() > 0)
        {

            SSTChildIntervalInfo<CHAR, INDEX> top = childStack.top();
            LCPInterval<INDEX> &childInterval = top.interval;

            if (x.i <= childInterval.i && childInterval.j <= x.j)
            {
                if(x_hasAtLeastOneChild){
                    if(top.hasSingleBWTCharacters){
                        if(top.bwt != x_BWTChar){
                            x_hasSingleBWTCharacter = false;
                        }
                    }else{
                        x_hasSingleBWTCharacter = false;
                    }
                }else{
                    if(top.hasSingleBWTCharacters){
                        x_hasSingleBWTCharacter = true;
                        x_BWTChar = top.bwt;
                    }else{
                        x_hasSingleBWTCharacter = false;
                    }                    
                    x_hasAtLeastOneChild = true;
                }

                if (childInterval.i == childInterval.j)
                {
                    outputQueue.push(SpecializedLCPInterval<CHAR, INDEX>(childInterval, id, top.bwt, top.charRank, top.hasSingleBWTCharacters));
                }
                else
                {
                    outputQueue.push(SpecializedLCPInterval<CHAR, INDEX>(childInterval, id, 0      , top.id,top.hasSingleBWTCharacters));
                }
                childStack.pop();
            }
            else
            {
                break;
            }
        }
        if (x.i == x.j)
        {
            while(bwt_index < x.i){
                ++_bwt_iterator;
                ++bwt_index;
            }
            CHAR bwtChar = *_bwt_iterator;
            if (this->charRankMap.find(bwtChar ) == this->charRankMap.end())
            {
                this->charRankMap[bwtChar] = 0;
            }
            else
            {
                this->charRankMap[bwtChar ]++;
            }
            childStack.push(SSTChildIntervalInfo<CHAR, INDEX>(x, id, this->charRankMap[bwtChar], bwtChar, true ));
        }
        else
        {
            if(x_hasSingleBWTCharacter){
            childStack.push(SSTChildIntervalInfo<CHAR, INDEX>(x, id, 0, x_BWTChar, true));
            //childStack.push(SSTChildIntervalInfo<CHAR, INDEX>(x, id, 0, x_BWTChar, false));
            }else{
            childStack.push(SSTChildIntervalInfo<CHAR, INDEX>(x, id, 0, 0, false));
            }
        }

        ++_iterator;
        //counter_i++;
        return !_iterator.isEnded();
    }
    bool succ()
    {
        while (this->outputQueue.size() == 0 && !_iterator.isEnded())
        {
            this->compute_next_special_interval();
        }
        if (_iterator.isEnded() && childStack.size() == 1)
        {
            SSTChildIntervalInfo<CHAR, INDEX> top = childStack.top();
            outputQueue.push(SpecializedLCPInterval<CHAR, INDEX>(top.interval, std::numeric_limits<INDEX>::max(), top.bwt, top.id, top.hasSingleBWTCharacters));
            childStack.pop();
        }

        if (this->outputQueue.size() > 0)
        {
            _currenct_lcp_interval = this->outputQueue.front();
            this->outputQueue.pop();
            return true;
        }
        else
        {
            _currenct_lcp_interval = SpecializedLCPInterval<CHAR, INDEX>::create_end_marker();

            return false;
        }
        return true;
    }

public:
    iterator() = default;

    iterator(BWT_ITERATOR &__bwt, ST_ITERATOR &__iterator, bool isBegin) : _iterator(__iterator), _bwt_iterator(__bwt)
    {
        if (isBegin)
        {
            this->succ();
        }
        else
        {
            this->_currenct_lcp_interval = SpecializedLCPInterval<CHAR, INDEX>::create_end_marker();
        }
    }

    //bool takeFront(LCPInterval &outputInterval);

    iterator &operator++()
    {
        this->succ();
        current_i++;
        return *this;
    }
    SpecializedLCPInterval<CHAR, INDEX> operator*()
    {
        return this->_currenct_lcp_interval;
    }
    bool operator!=(const iterator &rhs)
    {
        return _iterator != rhs._iterator;
    }
    INDEX get_current_i()
    {
        return this->current_i;
    }

    INDEX get_text_size()
    {
    return _iterator.get_text_size();
      }
    /*
    template <typename SA = std::vector<INDEX>, typename LCP = std::vector<INDEX> >
    static auto constructIterator(const std::vector<CHAR> &__bwt,const  SA &_SA,const  LCP &_LCPArray)
    {
        if (_SA[0] != _SA.size() - 1)
        {
            throw std::logic_error("The first value of SA must be the last position of the text.");
        }
        auto sa_beg = _SA.begin();
        auto lcp_beg = _LCPArray.begin();
        typename PostorderSuffixTreeIntervals<INDEX, SA, LCP>::template iterator<decltype(_SA.begin()) , decltype(_LCPArray.begin())> st(sa_beg, lcp_beg, true,_SA.size() );
        typename std::vector<CHAR>::const_iterator bwt_iterator = __bwt.begin();
        return iterator<typename std::vector<CHAR>::const_iterator, typename PostorderSuffixTreeIntervals<INDEX, SA, LCP>::template ) > > (bwt_iterator, st, true);
    }
    */
    bool isEnded()
    {
        return _currenct_lcp_interval.isEnd();
    }
};

 private:
  const BWT *_bwt;
  STI *_sti;
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

  void construct(const SA* __sa,const LCP* __lcp,const BWT* __bwt){
      this->_bwt = __bwt;
      this->_sti = new STI();
    this->_sti->construct(__sa, __lcp);
  }

  const BWT* get_bwt_pointer() const {
    return this->_bwt;
  }
  const SA* get_sa_pointer() const{
      return this->_sti->get_SA_pointer();
  }
  const LCP* get_lcp_pointer() const{
      return this->_sti->get_LCP_pointer();
  }
  
  const STI* get_sti_pointer() const {
    return this->_sti;
  }
  

  auto begin() const -> iterator< decltype(this->get_sa_pointer()->begin() ), decltype(this->get_lcp_pointer()->begin() ) ,  decltype(this->get_bwt_pointer()->begin() )>
  {
    using BWT_IT = decltype(this->get_bwt_pointer()->begin() );
    using SA_IT = decltype(this->get_sa_pointer()->begin() );
    using LCP_IT = decltype(this->get_lcp_pointer()->begin() );

    BWT_IT bwt_beg = this->_bwt->begin();
    auto sti_beg = this->get_sti_pointer()->begin();
    //LCP_IT sti_beg = this->get_sti_pointer()->get_LCP_pointer()->begin();


    auto it = iterator<SA_IT, LCP_IT, BWT_IT>(bwt_beg, sti_beg, true );
    return it;
  }
  auto end() const -> iterator< decltype(this->get_sa_pointer()->begin() ), decltype(this->get_lcp_pointer()->begin() ) ,  decltype(this->get_bwt_pointer()->begin() )>
  {    
    using BWT_IT = decltype(this->get_bwt_pointer()->begin() );
    using SA_IT = decltype(this->get_sa_pointer()->begin() );
    using LCP_IT = decltype(this->get_lcp_pointer()->begin() );

    BWT_IT bwt_beg = this->_bwt->begin();
    //using STI_IT = decltype(this->get_sti_pointer()->begin());
    auto sti_beg = this->get_sti_pointer()->begin();
    auto it = iterator<SA_IT, LCP_IT, BWT_IT>(bwt_beg, sti_beg, false );
    return it;
  }
};

/*
template <typename INDEX = uint64_t, typename VEC = std::vector<INDEX>>
class PostorderSpecialSuffixTree
{

    const VEC *_SA;
    const VEC *_LCPArray;
    bool deleteFlag = false;

public:
    PostorderSpecialSuffixTree(VEC &&__SA, VEC &&__LCPArray) : _SA(new VEC(std::move(__SA))), _LCPArray(new VEC(std::move(__LCPArray))), deleteFlag(true)
    {
    }
    PostorderSpecialSuffixTree() : deleteFlag(false)
    {
    }

    ~PostorderSpecialSuffixTree()
    {
        if (deleteFlag)
            delete _SA;
        delete _LCPArray;
    }

    PostorderSpecialSuffixTree(PostorderSpecialSuffixTree &&obj)
    {
        this->_SA = obj._SA;
        this->_LCPArray = obj._LCPArray;

        this->deleteFlag = obj.deleteFlag;
        obj.deleteFlag = false;
    }
    PostorderSpecialSuffixTree(const PostorderSpecialSuffixTree &obj)
    {
        if (obj._SA != nullptr)
        {
            throw std::logic_error("PostorderSpecialSuffixTree instances cannot call the copy constructor.");
        }
    }

    void set(VEC &&__SA, VEC &&__LCPArray)
    {
        this->_SA = new VEC(std::move(__SA));
        this->_LCPArray = new VEC(std::move(__LCPArray));

        deleteFlag = true;
    }

    PostorderSTIterator<INDEX, VEC> begin() const
    {
        //LCPIterator<INDEX, VEC> lcp_it = this->_lcp_generator->begin();
        auto it = PostorderSTIterator<INDEX, VEC>(this->_SA, this->_LCPArray, true);
        return it;
    }
    PostorderSTIterator<INDEX, VEC> end() const
    {
        //LCPIterator<INDEX, VEC> lcp_it = this->_lcp_generator->begin();
        auto it = PostorderSTIterator<INDEX, VEC>(this->_SA, this->_LCPArray, false);
        return it;
    }
    std::vector<LCPInterval<INDEX>> to_lcp_intervals() const
    {
        std::vector<LCPInterval<INDEX>> r;
        for (LCPInterval<INDEX> c : *this)
        {
            r.push_back(c);
        }
        return r;
    }
};
*/
} // namespace esaxx
} // namespace stool