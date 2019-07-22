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
#include "postorder_suffix_tree.hpp"
#include "postorder_special_suffix_tree.hpp"

#include "sa.hpp"

using namespace std;
using INDEXTYPE = uint64_t;
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
  //vector<char> T = stool::load_text(inputFile); // input text
  vector<char> T{'m', 'i', 's', 'i', 's', 's', 'i', 'p', 'p', 'i'};

  vector<uint64_t> sa = stool::constructSA<>(T);
  vector<uint64_t> lcpArray = stool::constructLCP<>(T, sa);
  vector<char> bwt = stool::constructBWT<>(T, sa);
  for (char c : bwt)
  {
    std::cout << std::string(1, c);
  }
  std::cout << std::endl;

  /*
    for(uint64_t i=0;i<sa.size();i++){
      std::cout << sa[i] << ",";
    }
    std::cout << std::endl;

    stool::PostorderSuffixTree<INDEXTYPE> st(std::move(sa), std::move(lcpArray));
    for(stool::LCPInterval<INDEXTYPE> itev : st){      
      std::cout << itev.toString()<< std::endl;
    }
    */

  std::cout << "-----" << std::endl;
  stool::PostorderSSTIterator<char, INDEXTYPE> st2 = stool::PostorderSSTIterator<char, INDEXTYPE>::constructIterator(bwt, sa, lcpArray);
  while (!st2.isEnd())
  {
    stool::SpecializedLCPInterval<INDEXTYPE> p = *st2;
    ++st2;
    std::cout << p.to_string() << std::endl;
  }

  //generator.set(std::move(sa), std::move(lcpArray) );

  //stool::PostorderLCPIntervals<INDEXTYPE> generator(T);
  /*
    uint64_t x = 0;
    for(stool::LCPInterval<INDEXTYPE> it : generator){
      x++;
        it.print(0, T, generator.SA);
        if(x > 40) throw -1; 
    }
    */
}