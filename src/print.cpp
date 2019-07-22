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

using namespace std;
using namespace stool;

using INDEXTYPE = int64_t;

int main(int argc, char *argv[])
{

    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("lcp_interval_file", 'l', "LCP interval file name", true);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string intervalFile = p.get<string>("lcp_interval_file");

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