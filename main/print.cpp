// License: MIT http://opensource.org/licenses/MIT

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "stool/include/light_stool.hpp"
#include "divsufsort.h"
#include "divsufsort64.h"
#include "libdivsufsort/sa.hpp"
#include "../include/common.hpp"

//#include "../minimal_substrings/naive_minimal_substrings.hpp"

using namespace std;
using namespace stool;

using INDEXTYPE = int64_t;
using CHAR = char;

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
template <typename T>
bool load_vector(std::string &filename, std::vector<T> &text, bool has_size_info, bool print_loading_message = true)
{

	if(print_loading_message)std::cout << "Loading: " << filename << std::endl;
	std::ifstream file;
	file.open(filename, std::ios::binary);

	if (!file)
	{
		std::cerr << "error reading file " << std::endl;
		return false;
	}

	if (has_size_info)
	{
		uint64_t size = UINT64_MAX;
		file.read((char *)&(size), sizeof(uint64_t));
		text.resize(size);
		file.read((char *)&(text)[0], size * sizeof(T));
		file.close();
		file.clear();
	}
	else
	{
		uint64_t len;
		file.seekg(0, std::ios::end);
		uint64_t n = (unsigned long)file.tellg();
		file.seekg(0, std::ios::beg);
		len = n / sizeof(T);

		text.resize(len);
		file.read((char *)&(text)[0], len * sizeof(T));
	}
	return true;
}

int main(int argc, char *argv[])
{

    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("lcp_interval_file", 'l', "LCP interval file name", true);
    //p.add<string>("tree_file", 't', "file type", false, "NULL");

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string intervalFile = p.get<string>("lcp_interval_file");
    //string type = p.get<string>("tree_file");

    std::vector<char> T = load_text(inputFile); // input text
    std::vector<LCPInterval<uint64_t>> intervals;
    load_vector<LCPInterval<uint64_t>>(intervalFile, intervals, false, true);

    std::vector<uint64_t> sa = stool::construct_suffix_array(T);
    stool::esaxx::print<char, uint64_t>(intervals, T, sa);
    stool::esaxx::printText<char>(T);
    stool::esaxx::printColor<char, uint64_t>(intervals, T, sa, true);

        /*
        std::cout << "id"
                  << "\t"
                  << "occurrence"
                  << "\t"
                  << "range(SA)"
                  << "\t"
                  << "string length"
                  << "\t"
                  << "string" << std::endl;
        for (uint64_t i = 0; i < intervals.size(); i++)
        {
            intervals[i].print(i, T, sa);
        }
        */
        

    return 0;
}