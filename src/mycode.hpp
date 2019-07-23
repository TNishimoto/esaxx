#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "divsufsort.h"
#include "divsufsort64.h"

using namespace std;
namespace stool
{

template <typename index_type>
class LCPInterval
{
public:
	index_type i;
	index_type j;
	index_type lcp;
	LCPInterval() {}
	LCPInterval(index_type _i, index_type _j, index_type _lcp)
	{
		this->i = _i;
		this->j = _j;
		this->lcp = _lcp;
	}

	static LCPInterval<index_type> create_end_marker(){
		return LCPInterval<index_type>(std::numeric_limits<index_type>::max(),std::numeric_limits<index_type>::max(),std::numeric_limits<index_type>::max());
	}
	/*
	bool operator<(const LCPInterval<index_type> &right) const
    {
        if (this->i == right.i)
        {
            if (this->j == right.j)
            {
                return this->lcp < right.lcp;
            }
            else
            {
                return this->j < right.j;
            }
        }
        else
        {
            return this->i < right.i;
        }
    }
	*/
    std::string to_string()
    {
        return "[" + std::to_string(i) + ", " + std::to_string(j) + ", " + std::to_string(lcp) + "]";
    }
	bool is_special_marker(){
		return this->i == std::numeric_limits<index_type>::max() && this->j == std::numeric_limits<index_type>::max() && this->lcp == std::numeric_limits<index_type>::max();
	}

	template <typename sa_type>
	void print(uint64_t id, vector<char> &text, sa_type &sa)
	{
		cout << id << "\t\t" << (this->j - this->i + 1) << "\t\t" << this->i << ".." << this->j << "\t\t" << this->lcp << "\t\t";
		int64_t begin = sa[this->i];
		for (int64_t j = 0; j < this->lcp; ++j)
		{
			if(text[begin + j] != 0){
				cout << text[begin + j];
			}else{
				cout << "$(special end character)";
			}
		}
		cout << endl;
	}
	template <typename sa_type>
	string getCSVLine(uint64_t id, vector<char> &text, sa_type &sa){
		string tmp="";
		string intervalText = this->getText(text, sa);
		tmp = std::to_string(id) + "," + std::to_string(this->j - this->i + 1) + "," + std::to_string(this->i) + "," + std::to_string(this->j) + "," + intervalText;
		return tmp;
	}

	template <typename sa_type>
	string getText(vector<char> &text, sa_type &sa){
		string intervalText = "";
		int64_t begin = sa[this->i];
		for (int64_t j = 0; j < this->lcp; ++j)
		{
			intervalText.push_back(text[begin + j]);
		}
		return intervalText;
	}

};

/**
 * @brief Constructs the suffix array of a given string in linear time.
 * @param T[0..n-1] The input string. (random access iterator)
 * @param SA[0..n-1] The output array of suffixes. (random access iterator)
 * @param n The length of the given string.
 * @param k The alphabet size.
 * @return 0 if no error occurred, -1 or -2 otherwise.
 */
template <typename string_type, typename sarray_type, typename index_type>
int saisxx(string_type T, sarray_type SA, index_type n)
{
}
template <>
int saisxx<std::vector<char>::iterator, std::vector<int32_t>::iterator, int32_t>(std::vector<char>::iterator T, std::vector<int32_t>::iterator SA, int32_t n)
{
	if (n > 0 && T[n - 1] != 0)
	{
		throw std::logic_error("The last character of the input text must be '0'");
	}
	//if((n < 0) || (k <= 0)) { return -1; }
	if (n <= 1)
	{
		if (n == 1)
		{
			SA[0] = 0;
		}
		return 0;
	}

	int err = divsufsort((const unsigned char *)&T[0], (int32_t *)&SA[0], n);
	return err;
}
template <>
int saisxx<std::vector<char>::iterator, std::vector<int64_t>::iterator, int64_t>(std::vector<char>::iterator T, std::vector<int64_t>::iterator SA, int64_t n)
{
	//if((n < 0) || (k <= 0)) { return -1; }
	if (n > 0 && T[n - 1] != 0)
	{
		throw std::logic_error("The last character of the input text must be '0'");
	}

	if (n <= 1)
	{
		if (n == 1)
		{
			SA[0] = 0;
		}
		return 0;
	}

	int err = divsufsort64((const unsigned char *)&T[0], (int64_t *)&SA[0], n);
	return err;
}

template <typename T>
static vector<T> load(string filename)
{

	ifstream stream;
	stream.open(filename, ios::binary);

	vector<T> vec;

	if (!stream)
	{
		std::cerr << "error reading file " << endl;
		throw - 1;
	}
	uint64_t len;
	stream.seekg(0, ios::end);
	uint64_t n = (unsigned long)stream.tellg();
	stream.seekg(0, ios::beg);
	len = n / sizeof(T);

	vec.resize(len);
	stream.read((char *)&(vec)[0], len * sizeof(T));
	return vec;
}

static vector<char> load_text(string filename)
{

	ifstream stream;
	stream.open(filename, ios::binary);

	vector<char> vec;

	if (!stream)
	{
		std::cerr << "error reading file " << endl;
		throw - 1;
	}
	uint64_t len;
	stream.seekg(0, ios::end);
	uint64_t n = (unsigned long)stream.tellg();
	stream.seekg(0, ios::beg);
	len = n / sizeof(char);

	vec.resize(len+1, 0);
	stream.read((char *)&(vec)[0], len * sizeof(char));

	for(uint64_t i=0;i<len;i++){
		if(vec[i] == 0){
			throw std::logic_error("The input text must not contain '0' character!");
		}		
	}
	return vec;
}


vector<int64_t> construct_sa(vector<char> &text)
{
	vector<int64_t> SA;
	int64_t n = text.size();
	SA.resize(n);

	divsufsort64((const unsigned char *)&text[0], (int64_t *)&SA[0], n);

	return SA;
}

} // namespace stool