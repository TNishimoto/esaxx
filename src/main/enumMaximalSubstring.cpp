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

using namespace std;
using CHAR = uint8_t;
using INDEXTYPE = int64_t;

int main(int argc, char *argv[])
{

  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);
  p.add<string>("output_file", 'o', "output file name", false, "");
  p.add<bool>("print", 'p', "print info", false, true);
  p.add<string>("format", 'f', "output format (binary or csv)", false, "binary");

  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");
  string outputFile = p.get<string>("output_file");
  string format = p.get<string>("format");

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

  vector<char> T = stool::load_text(inputFile); // input text
  //vector<uint8_t> T = stool::load_text_from_file(inputFile, true); // input text
  //INDEXTYPE n = T.size();
  vector<INDEXTYPE> SA; // suffix array


  stool::PostorderMaximalSubstrings<INDEXTYPE> iterator = stool::PostorderMaximalSubstrings<INDEXTYPE>::construct(T, SA);
    std::vector<stool::LCPInterval<INDEXTYPE>> intervals;
    for (auto it : iterator)
    {
      intervals.push_back(it);
    }
    if (isPrint)
    {
      std::cout << "Maximal substrings in the file" << std::endl;
      stool::esaxx::print<char, INDEXTYPE>(intervals, T, SA );
    }

  if (format != "binary")
  {
    stool::esaxx::writeText<char, INDEXTYPE>(outputFile, intervals, T, SA );
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