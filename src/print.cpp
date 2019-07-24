// License: MIT http://opensource.org/licenses/MIT

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "cmdline.h"
#include "divsufsort.h"
#include "divsufsort64.h"
#include "mycode.hpp"
#include "minimal_substrings/minimal_substring_tree.hpp"
#include "minimal_substrings/naive_minimal_substrings.hpp"

using namespace std;
using namespace stool;

using INDEXTYPE = int64_t;

int main(int argc, char *argv[])
{

    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("lcp_interval_file", 'l', "LCP interval file name", true);
    p.add<string>("tree_file", 't', "file type", false, "NULL");

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string intervalFile = p.get<string>("lcp_interval_file");
    string type = p.get<string>("tree_file");

    if (type != "NULL")
    {
        
        vector<uint8_t> T = stool::load_text2(inputFile); // input text
        stool::MinimalSubstringTree<uint8_t, uint64_t> mstree;
        mstree.load(type, T);
        std::cout << "load!" << std::endl;
        T.pop_back();
        vector<uint64_t> sa = stool::constructSA<>(T);
        string otext = "";

        for (uint64_t i = 0; i < mstree.nodes.size(); i++)
        {
            otext.append(stool::toLogLine<>(T, sa, mstree.nodes[i]));
            if (i + 1 != mstree.nodes.size())
                otext.append("\r\n");
        }
        return 0;
    }
    else
    {
        vector<char> T = stool::load_text(inputFile); // input text

        vector<LCPInterval<INDEXTYPE>> intervals = stool::load<LCPInterval<INDEXTYPE>>(intervalFile);
        vector<int64_t> SA = stool::construct_sa(T);
        std::cout << "id"
                  << "\t\t"
                  << "occurrence"
                  << "\t"
                  << "range(SA)"
                  << "\t"
                  << "string length"
                  << "\t"
                  << "string" << std::endl;
        for (uint64_t i = 0; i < intervals.size(); i++)
        {
            intervals[i].print(i, T, SA);
        }

        return 0;
    }
}