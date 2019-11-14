// License: MIT http://opensource.org/licenses/MIT
/*
  This code was copied from https://takeda25.hatenablog.jp/entry/20101202/1291269994 and I modified it.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "stool/src/cmdline.h"
#include "../esa.hxx"
#include "../postorder_maximal_substrings.hpp"
#include "common.hpp"
#include "libdivsufsort/sa.hpp"
#include "../postorder_maximal_substring_intervals.hpp"

using namespace std;
using CHAR = char;
using INDEXTYPE = uint64_t;

/*
void iterateMSwithSDSL(){

}
void iterateMS(){

}
*/
int main(int argc, char *argv[])
{

  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);
  p.add<string>("output_file", 'o', "output file name", false, "");
  p.add<bool>("print", 'p', "print info", false, true);
  p.add<string>("format", 'f', "output format (binary or csv)", false, "binary");
  p.add<string>("mode", 'm', "mode(esaxx or succinct)", false, "esaxx");


  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");
  string outputFile = p.get<string>("output_file");
  string format = p.get<string>("format");
  string mode = p.get<string>("mode");

  if (format != "binary")
  {
    format = "csv";
  }

  bool isPrint = p.get<bool>("print");

  if (outputFile.size() == 0)
  {
    if(format == "csv"){
      outputFile = inputFile + ".max.csv";
    }else{
      outputFile = inputFile + ".max";
    }
  }

  //vector<char> T = stool::load_text(inputFile); // input text
  vector<CHAR> T = stool::load_char_vec_from_file(inputFile, true); // input text
  //INDEXTYPE n = T.size();

  std::vector<INDEXTYPE> sa = stool::construct_suffix_array(T);
  std::cout << "Constructing LCP Array" << std::endl;
  std::vector<INDEXTYPE> lcpArray = stool::constructLCP<CHAR, INDEXTYPE>(T, sa);

  std::vector<CHAR> bwt = stool::esaxx::constructBWT<CHAR, INDEXTYPE, std::vector<INDEXTYPE>>(T, sa);
  std::cout << "Constructing PST" << std::endl;
  //vector<INDEXTYPE> SA; // suffix array

  stool::PostorderMaximalSubstringIntervals<CHAR, INDEXTYPE, std::vector<INDEXTYPE>, std::vector<INDEXTYPE>, std::vector<CHAR> > pmsi;
  pmsi.construct(&sa, &lcpArray, &bwt);

  //stool::PostorderMaximalSubstrings<INDEXTYPE> iterator = stool::PostorderMaximalSubstrings<INDEXTYPE>::construct(T, sa);
    std::vector<stool::LCPInterval<INDEXTYPE>> intervals;
    for (auto it : pmsi)
    {
      intervals.push_back(it);
    }
    if (isPrint)
    {
      std::cout << "Maximal substrings in the file" << std::endl;
      stool::esaxx::print<CHAR, INDEXTYPE>(intervals, T, sa );
    }

  if (format != "binary")
  {
    stool::esaxx::writeText<CHAR, INDEXTYPE>(outputFile, intervals, T, sa );
  }
  else
  {

    stool::write_vector(outputFile, intervals, false);
  }


  std::cout << "\033[36m";
  std::cout << "___________RESULT___________" << std::endl;
  std::cout << "File: " << inputFile << std::endl;
  std::cout << "Output: " << outputFile << std::endl;
  std::cout << "Output format: " << format << std::endl;
  std::cout << "The length of the input text: " << T.size() << std::endl;
  //std::cout << "The number of maximum substrings: " << maximumSubstringCount << std::endl;
  std::cout << "The number of maximum substrings: " << intervals.size() << std::endl;
  std::cout << "_________________________________" << std::endl;
  std::cout << "\033[39m" << std::endl;
}