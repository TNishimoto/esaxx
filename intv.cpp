// License: MIT http://opensource.org/licenses/MIT
/*
  This code was copied from https://takeda25.hatenablog.jp/entry/20101202/1291269994 and I modified it.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "cmdline.h"
#include "esa.hxx"
#include "lcp_interval_iterator.hpp"

using namespace std;
using INDEXTYPE = int64_t;
int main(int argc, char *argv[])
{
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    //p.add<string>("output_file", 'o', "output file name", false, "");
    //p.add<bool>("print", 'p', "print info", false, true);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    //string outputFile = p.get<string>("output_file");
    //bool isPrint = p.get<bool>("print");
    vector<char> T = stool::load_text(inputFile); // input text

    stool::LCPIntervalIteratorGenerator<INDEXTYPE> generator(T);

    uint64_t x = 0;
    for(stool::LCPInterval<INDEXTYPE> it : generator){
      x++;
        it.print(0, T, generator.SA);
        if(x > 40) throw -1; 
    }
}