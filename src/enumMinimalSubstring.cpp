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
#include "postorder_maximal_substrings.hpp"

using namespace std;
using INDEXTYPE = int64_t;

int main(int argc, char *argv[])
{

  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);
  p.add<string>("output_file", 'o', "output file name", false, "");
  p.add<bool>("print", 'p', "print info", false, true);
  p.add<string>("format", 'f', "output format", false, "csv");

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
    outputFile = inputFile + ".ms";
  }

  vector<char> T = stool::load_text(inputFile); // input text
  INDEXTYPE n = T.size();
  vector<INDEXTYPE> SA; // suffix array

  stool::PostorderMaximalSubstrings<INDEXTYPE> iterator = stool::PostorderMaximalSubstrings<INDEXTYPE>::construct(T,SA);
  
  if (isPrint)
  {
    std::cout << "Maximal substrings in the file" << std::endl;
    std::cout << "id"
              << "\t\t"
              << "occurrence"
              << "\t"
              << "range(SA)"
              << "\t"
              << "string length"
              << "\t"
              << "string" << std::endl;
  }

  vector<stool::LCPInterval<INDEXTYPE>> buffer;
  ofstream os(outputFile, ios::out | ios::binary);
  if (!os)
    return 1;
  INDEXTYPE maximumSubstringCount = 0;
  INDEXTYPE id = 0;

  // Filtering internal nodes and writing and printing maximal substrings.
  INDEXTYPE line_id = 0;

  for (auto it : iterator)
  {
    id++;
    maximumSubstringCount++;
    buffer.push_back(it);
    //std::cout << it.i << "/" << it.j << "/" << it.lcp << std::endl;

    if (buffer.size() > 8192)
    {
      if (format == "binary")
      {
        os.write((const char *)(&buffer[0]), sizeof(stool::LCPInterval<INDEXTYPE>) * buffer.size());
      }
      else
      {
        for (uint64_t z = 0; z < buffer.size(); z++)
        {
          os << buffer[z].getCSVLine(line_id, T, SA) << std::endl;
          line_id++;
        }
      }
      buffer.clear();
    }

    if (isPrint)
    {
      if (maximumSubstringCount < 1000)
      {
        it.print(id, T, SA);
      }
      else if (maximumSubstringCount == 1000)
      {
        std::cout << "etc.." << std::endl;
      }
    }
  }
  buffer.clear();
  os.close();

  std::cout << "\033[36m";
  std::cout << "___________RESULT___________" << std::endl;
  std::cout << "File: " << inputFile << std::endl;
  std::cout << "Output: " << outputFile << std::endl;
  std::cout << "Output format: " << format << std::endl;
  std::cout << "The length of the input text: " << T.size() << std::endl;
  //std::cout << "The number of maximum substrings: " << maximumSubstringCount << std::endl;
  std::cout << "The number of maximum substrings: " << iterator.size() << std::endl;
  std::cout << "_________________________________" << std::endl;
  std::cout << "\033[39m" << std::endl;
}