#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "cmdline.h"
#include "esa.hxx"
#include <exception>
using namespace std;

namespace stool
{
template <typename INDEXTYPE>
class PostorderMSIterator
{
    const vector<INDEXTYPE> &L; // left boundaries of internal node
    const vector<INDEXTYPE> &R; // right boundaries of internal node
    const vector<INDEXTYPE> &D; // depths of internal node
    INDEXTYPE index;
    public:
    PostorderMSIterator() = default;
    PostorderMSIterator(const vector<INDEXTYPE> &_L, const vector<INDEXTYPE> &_R, const vector<INDEXTYPE> &_D, INDEXTYPE _index) : L(_L), R(_R), D(_D), index(_index)
    {
    }

    PostorderMSIterator &operator++()
    {
        if (index + 1 < this->L.size())
        {
            this->index++;
        }
        else
        {
            this->index = std::numeric_limits<INDEXTYPE>::max();
        }
        return *this;
    }
    stool::LCPInterval<INDEXTYPE> operator*()
    {
        return stool::LCPInterval<INDEXTYPE>(L[index], R[index], D[index]);
    }
    bool operator!=(const PostorderMSIterator &rhs)
    {
        return index != rhs.index;
    }
};
template <typename INDEXTYPE>
class PostorderMaximalSubstrings
{

    vector<INDEXTYPE> L; // left boundaries of internal node
    vector<INDEXTYPE> R; // right boundaries of internal node
    vector<INDEXTYPE> D; // depths of internal node
    vector<INDEXTYPE> rank;

public:
    PostorderMaximalSubstrings()
    {
    }
    template <typename TEXT>
    static PostorderMaximalSubstrings<INDEXTYPE> construct(vector<TEXT> &text, vector<INDEXTYPE> &SA)
    {
        PostorderMaximalSubstrings<INDEXTYPE> r;
        INDEXTYPE n = text.size();
        SA.resize(n);
        //r.SA.resize(n);
        r.L.resize(n);
        r.R.resize(n);
        r.D.resize(n);
        vector<INDEXTYPE> rank(n);
        INDEXTYPE alphaSize = 0x100; // This can be very large
        INDEXTYPE nodeNum = 0;
        INDEXTYPE SA_first_index = 0;

        // Computing internal nodes of the suffix tree of the input file.
        if (esaxx(text.begin(), SA.begin(),
                  r.L.begin(), r.R.begin(), r.D.begin(),
                  n, alphaSize, nodeNum) == -1)
        {
            throw logic_error("error");
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
            if ((rank[r.R[i] - 1] - rank[r.L[i]] == 0))
            {
                continue;
            }
            else
            {
                r.L[y] = r.L[i];
                r.R[y] = r.R[i];
                r.D[y] = r.D[i];
                y++;
            }
        }
        y;
        r.L.resize(y+1);        
        r.R.resize(y+1);
        r.D.resize(y+1);
        while(y >= 1){
            r.L[y] = r.L[y-1];
            r.R[y] = r.R[y-1];
            r.D[y] = r.D[y-1];
            y--;
        }

        r.L[0] = (SA_first_index);
        r.R[0] = (SA_first_index);
        r.D[0] = (n);
        return r;
    }

    PostorderMSIterator<INDEXTYPE> begin()
    {
        auto it = PostorderMSIterator<INDEXTYPE>(this->L, this->R, this->D, 0);
        return it;
    }
    PostorderMSIterator<INDEXTYPE> end()
    {
        auto it = PostorderMSIterator<INDEXTYPE>(this->L, this->R, this->D, std::numeric_limits<INDEXTYPE>::max());
        return it;
    }
    INDEXTYPE size(){
        return this->L.size();
    }
};

} // namespace stool