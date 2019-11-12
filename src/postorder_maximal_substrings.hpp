#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "stool/src/cmdline.h"
#include "esa.hxx"
#include <exception>
#include "stool/src/io.hpp"
#include "stool/src/sa_bwt_lcp.hpp"



namespace stool
{

class DistinctCharacterCheckerOnInterval{
    std::vector<uint64_t> vec;
    public:
    bool contains(uint64_t left, uint64_t right) const {
        return vec[right]-vec[left] != 0;
    }

    template <typename CHAR, typename TEXT, typename SA>
    void construct(TEXT &text, SA &sa){
        vec.resize(text.size(), 0);
        for(uint64_t i=1;i<text.size();i++){
            uint64_t p = sa[i];
            CHAR prev_c = p == 0 ? text[text.size()-1] : text[p-1];
            CHAR c = text[p];
            if(c != prev_c){
                vec[i] = vec[i-1]+1;
            }else{
                vec[i] = vec[i-1];
            }
        }
    }
};


template <typename INDEXTYPE>
class PostorderMSIterator
{
    const std::vector<INDEXTYPE> &L; // left boundaries of internal node
    const std::vector<INDEXTYPE> &R; // right boundaries of internal node
    const std::vector<INDEXTYPE> &D; // depths of internal node
    INDEXTYPE index;
    public:
    PostorderMSIterator() = default;
    PostorderMSIterator(const std::vector<INDEXTYPE> &_L, const std::vector<INDEXTYPE> &_R, const std::vector<INDEXTYPE> &_D, INDEXTYPE _index) : L(_L), R(_R), D(_D), index(_index)
    {
    }
    PostorderMSIterator &operator++()
    {
        if (index + 1 < (INDEXTYPE)this->L.size())
        {
            this->index++;
        }
        else
        {
            this->index = std::numeric_limits<INDEXTYPE>::max();
        }
        return *this;
    }
    stool::LCPInterval<INDEXTYPE> operator*() const
    {
        return stool::LCPInterval<INDEXTYPE>(L[index], R[index], D[index]);
    }
    bool operator!=(const PostorderMSIterator &rhs) const
    {
        return index != rhs.index;
    }
};

template <typename INDEXTYPE, typename POSTORDER_ST_ITERATOR, typename DISTINCT_CHARACTER_CHECKER>
class PostorderMaximalSubstringIterator
{
    POSTORDER_ST_ITERATOR &st;
    const DISTINCT_CHARACTER_CHECKER &distinct_character_checker;
    INDEXTYPE index;
    stool::LCPInterval<INDEXTYPE> currentMSInterval;
    public:
    PostorderMaximalSubstringIterator() = default;
    PostorderMaximalSubstringIterator(POSTORDER_ST_ITERATOR &_st, const DISTINCT_CHARACTER_CHECKER &_distinct_character_checker) : st(_st), distinct_character_checker(_distinct_character_checker)
    {
        if(st.isEnded()){
            this->index = std::numeric_limits<INDEXTYPE>::max();
        }else{
            this->index = 0;
            ++(*this);
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
    PostorderMaximalSubstringIterator &operator++()
    {
        ++st;
        while(!st.isEnded()){
                auto interval = *st;
                if(distinct_character_checker.contains(interval.i, interval.j)){
                    //this->currentMSInterval = it;
                    index++;
                    break;
                }else{
                    ++st;
                }
        }
        if(st.isEnded()){
            this->index = std::numeric_limits<INDEXTYPE>::max();
        }
        return *this;
    }
    stool::LCPInterval<INDEXTYPE> operator*() const
    {
        return *st;
    }
    bool operator!=(const PostorderMaximalSubstringIterator &rhs) const
    {
        return index != rhs.index;
    }
};



template <typename INDEXTYPE>
class PostorderMaximalSubstrings
{

    const std::vector<INDEXTYPE> L; // left boundaries of internal node
    const std::vector<INDEXTYPE> R; // right boundaries of internal node
    const std::vector<INDEXTYPE> D; // depths of internal node

public:
    PostorderMaximalSubstrings()
    {
    }
    PostorderMaximalSubstrings(std::vector<INDEXTYPE> _L, std::vector<INDEXTYPE> _R, std::vector<INDEXTYPE> _D) : L(_L), R(_R), D(_D)
    {
    }

    template <typename TEXT>
    static PostorderMaximalSubstrings<INDEXTYPE> construct(std::vector<TEXT> &text, std::vector<INDEXTYPE> &SA)
    {
        INDEXTYPE n = text.size();
        SA.resize(n);
        std::vector<INDEXTYPE> L(n);
        std::vector<INDEXTYPE> R(n);
        std::vector<INDEXTYPE> D(n);
        //r.SA.resize(n);
        L.resize(n);
        R.resize(n);
        D.resize(n);
        std::vector<INDEXTYPE> rank(n);
        INDEXTYPE alphaSize = 0x100; // This can be very large
        INDEXTYPE nodeNum = 0;
        INDEXTYPE SA_first_index = 0;

        // Computing internal nodes of the suffix tree of the input file.
        if (esaxx(text.begin(), SA.begin(),
                  L.begin(), R.begin(), D.begin(),
                  n, alphaSize, nodeNum) == -1)
        {
            throw std::logic_error("error");
        }

        INDEXTYPE x = 0;
        for (INDEXTYPE i = 0; i < n; i++)
        {
            if (SA[i] == 0)
                SA_first_index = i;
            if (i == 0 || text[(SA[i] + n - 1) % n] != text[(SA[i - 1] + n - 1) % n])
            {
                x++;
            }
            rank[i] = x;
        }

        INDEXTYPE y = 0;
        for (INDEXTYPE i = 0; i < nodeNum; i++)
        {
            //stool::LCPInterval<INDEXTYPE> interval(r.L[i], r.R[i], r.D[i]);
            if ((rank[R[i] - 1] - rank[L[i]] == 0))
            {
                continue;
            }
            else
            {
                L[y] = L[i];
                R[y] = R[i];
                D[y] = D[i];
                y++;
            }
        }
        //y;
        L.resize(y+1);        
        R.resize(y+1);
        D.resize(y+1);
        while(y >= 1){
            L[y] = L[y-1];
            R[y] = R[y-1];
            D[y] = D[y-1];
            y--;
        }

        L[0] = (SA_first_index);
        R[0] = (SA_first_index);
        D[0] = (n);


        PostorderMaximalSubstrings<INDEXTYPE> r(std::move(L), std::move(R), std::move(D) );
        return r;
    }

    PostorderMSIterator<INDEXTYPE> begin() const
    {
        auto it = PostorderMSIterator<INDEXTYPE>(this->L, this->R, this->D, 0);
        return it;
    }
    PostorderMSIterator<INDEXTYPE> end() const
    {
        auto it = PostorderMSIterator<INDEXTYPE>(this->L, this->R, this->D, std::numeric_limits<INDEXTYPE>::max());
        return it;
    }
    INDEXTYPE size() const {
        return this->L.size();
    }
    stool::LCPInterval<INDEXTYPE> operator[](INDEXTYPE i) {
        return stool::LCPInterval<INDEXTYPE>(L[i], R[i], D[i]);
        
    }
};

} // namespace stool