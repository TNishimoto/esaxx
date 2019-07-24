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
#include "minimal_substrings/postorder_special_suffix_tree.hpp"
#include "minimal_substrings/minimal_substring_iterator.hpp"
#include "minimal_substrings/naive_minimal_substrings.hpp"

#include "sa.hpp"

using namespace std;
using INDEXTYPE = uint64_t;
int main(int argc, char *argv[])
{
  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);
  p.add<string>("output_file", 'o', "output file name", false, "");
  //p.add<string>("output_file", 'o', "output file name", false, "");
  //p.add<bool>("print", 'p', "print info", false, true);

  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");
  vector<uint8_t> T = stool::load_text2(inputFile); // input text
  std::cout << (int)T[T.size() - 1] << std::endl;


  vector<uint64_t> sa = stool::constructSA<>(T);
  vector<uint64_t> lcpArray = stool::constructLCP<>(T, sa);
  vector<uint8_t> bwt = stool::constructBWT<uint8_t>(T, sa);

  std::vector<stool::LCPInterval<INDEXTYPE>> msVec = stool::MinimalSubstringIterator<uint8_t, INDEXTYPE>::constructSortedMinimalSubstrings(bwt, sa, lcpArray);
  /* 
  for (uint64_t i = 0; i < msVec.size(); i++)
  {
    msVec[i].print(i, T, sa);
  }
  */

  /*
  for (auto s : testMS)
  {
    std::cout << s << std::endl;
  }

  */
  vector<char> T2(T.begin(),T.end()); // input text
  vector<string> testMS = stool::naive_compute_minimal_substrings<>(T2);
  std::cout << testMS.size() << std::endl;
  std::cout << msVec.size() << std::endl;
  if (testMS.size() != msVec.size())
  {
    throw std::logic_error("different size!");
  }


  std::vector<stool::LCPInterval<INDEXTYPE>> msVec2 = stool::MinimalSubstringIterator<uint8_t, INDEXTYPE>::constructSortedMinimalSubstringsWithoutSpecialMarker(bwt, sa, lcpArray);
  std::cout << "###" << msVec2.size() << std::endl;
  
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