#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "divsufsort.h"
#include "divsufsort64.h"

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

	static LCPInterval<index_type> create_end_marker()
	{
		return LCPInterval<index_type>(std::numeric_limits<index_type>::max(), std::numeric_limits<index_type>::max(), std::numeric_limits<index_type>::max());
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
	bool is_special_marker()
	{
		return this->i == std::numeric_limits<index_type>::max() && this->j == std::numeric_limits<index_type>::max() && this->lcp == std::numeric_limits<index_type>::max();
	}

	template <typename sa_type>
	void print(uint64_t id, std::vector<char> &text, sa_type &sa)
	{
		std::cout << id << "\t\t" << (this->j - this->i + 1) << "\t\t" << this->i << ".." << this->j << "\t\t" << this->lcp << "\t\t";
		int64_t begin = sa[this->i];
		for (int64_t j = 0; j < this->lcp; ++j)
		{
			if (text[begin + j] != 0)
			{
				std::cout << text[begin + j];
			}
			else
			{
				std::cout << "$(special end character)";
			}
		}
		std::cout << std::endl;
	}
	template <typename sa_type>
	std::string getCSVLine(uint64_t id, std::vector<char> &text, sa_type &sa)
	{
		std::string tmp = "";
		std::string intervalText = this->getText(text, sa);
		tmp = std::to_string(id) + "," + std::to_string(this->j - this->i + 1) + "," + std::to_string(this->i) + "," + std::to_string(this->j) + "," + intervalText;
		return tmp;
	}

	template <typename sa_type>
	std::string getText(std::vector<char> &text, sa_type &sa)
	{
		std::string intervalText = "";
		int64_t begin = sa[this->i];
		for (int64_t j = 0; j < this->lcp; ++j)
		{
			intervalText.push_back(text[begin + j]);
		}
		return intervalText;
	}
};


template <typename T>
std::vector<T> load(std::string filename)
{

	std::ifstream stream;
	stream.open(filename, std::ios::binary);

	std::vector<T> vec;

	if (!stream)
	{
		std::cerr << "error reading file " << std::endl;
		throw - 1;
	}
	uint64_t len;
	stream.seekg(0, std::ios::end);
	uint64_t n = (unsigned long)stream.tellg();
	stream.seekg(0, std::ios::beg);
	len = n / sizeof(T);

	vec.resize(len);
	stream.read((char *)&(vec)[0], len * sizeof(T));
	return vec;
}

std::vector<char> load_text(std::string filename)
{

	std::ifstream stream;
	stream.open(filename, std::ios::binary);

	std::vector<char> vec;

	if (!stream)
	{
		std::cerr << "error reading file " << std::endl;
		throw - 1;
	}
	uint64_t len;
	stream.seekg(0, std::ios::end);
	uint64_t n = (unsigned long)stream.tellg();
	stream.seekg(0, std::ios::beg);
	len = n / sizeof(char);

	vec.resize(len + 1, 0);
	stream.read((char *)&(vec)[0], len * sizeof(char));

	for (uint64_t i = 0; i < len; i++)
	{
		if (vec[i] == 0)
		{
			throw std::logic_error("The input text must not contain '0' character!");
		}
	}
	return vec;
}
std::vector<uint8_t> load_text2(std::string filename)
{

	std::ifstream stream;
	stream.open(filename, std::ios::binary);

	std::vector<uint8_t> vec;

	if (!stream)
	{
		std::cerr << "error reading file " << std::endl;
		throw - 1;
	}
	uint64_t len;
	stream.seekg(0, std::ios::end);
	uint64_t n = (unsigned long)stream.tellg();
	stream.seekg(0, std::ios::beg);
	len = n / sizeof(uint8_t);

	vec.resize(len + 1, 0);
	stream.read((char *)&(vec)[0], len * sizeof(char));

	for (uint64_t i = 0; i < len; i++)
	{
		if (vec[i] == 0)
		{
			throw std::logic_error("The input text must not contain '0' character!");
		}
	}
	return vec;
}

std::vector<int64_t> construct_sa(std::vector<char> &text)
{
	std::vector<int64_t> SA;
	int64_t n = text.size();
	SA.resize(n);

	divsufsort64((const unsigned char *)&text[0], (int64_t *)&SA[0], n);

	return SA;
}

} // namespace stool