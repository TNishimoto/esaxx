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

using namespace std;
using INDEXTYPE = int64_t;

int main(int argc, char *argv[])
{

  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);
  p.add<string>("output_file", 'o', "output file name", false, "");
  p.add<bool>("print", 'p', "print info", false, true);

  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");
  string outputFile = p.get<string>("output_file");
  bool isPrint = p.get<bool>("print");

  if (outputFile.size() == 0)
  {
    outputFile = inputFile + ".ms";
  }

  vector<char> T = stool::load_text(inputFile); // input text
  INDEXTYPE n = T.size();

  vector<INDEXTYPE> SA(n); // suffix array
  vector<INDEXTYPE> L(n);  // left boundaries of internal node
  vector<INDEXTYPE> R(n);  // right boundaries of internal node
  vector<INDEXTYPE> D(n);  // depths of internal node

  INDEXTYPE alphaSize = 0x100; // This can be very large
  INDEXTYPE nodeNum = 0;

  // Computing internal nodes of the suffix tree of the input file.
  if (esaxx(T.begin(), SA.begin(),
            L.begin(), R.begin(), D.begin(),
            n, alphaSize, nodeNum) == -1)
  {
    return -1;
  }

  INDEXTYPE size = T.size();
  INDEXTYPE SA_first_index = 0;

  vector<INDEXTYPE> rank(size);
  INDEXTYPE r = 0;
  for (INDEXTYPE i = 0; i < size; i++)
  {
    if (SA[i] == 0)
      SA_first_index = i;
    if (i == 0 || T[(SA[i] + size - 1) % size] != T[(SA[i - 1] + size - 1) % size])
    {
      r++;
    }
    rank[i] = r;
  }

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
  for (INDEXTYPE i = 0; i < nodeNum; ++i)
  {
    stool::LCPInterval<INDEXTYPE> interval(L[i], R[i], D[i]);
    INDEXTYPE len = D[i];
    if ((rank[interval.j - 1] - rank[interval.i] == 0))
    {
      continue;
    }
    id++;
    maximumSubstringCount++;
    buffer.push_back(interval);
    if (buffer.size() > 8192)
    {
      os.write((const char *)(&buffer[0]), sizeof(stool::LCPInterval<INDEXTYPE>) * buffer.size());
      buffer.clear();
    }

    if (isPrint)
    {
      if (maximumSubstringCount < 1000)
      {
        interval.print(id, T, SA);
      }
      else if (maximumSubstringCount == 1000)
      {
        std::cout << "etc.." << std::endl;
      }
    }
  }
  stool::LCPInterval<INDEXTYPE> interval(SA_first_index, SA_first_index, T.size());
  id++;
   maximumSubstringCount++;
  if (isPrint)
  {
    interval.print(id, T, SA);
  }
  buffer.push_back(interval);



  os.write((const char *)(&buffer[0]), sizeof(stool::LCPInterval<INDEXTYPE>) * buffer.size());
  buffer.clear();
  os.close();

  std::cout << "\033[36m";
  std::cout << "___________RESULT___________" << std::endl;
  std::cout << "File: " << inputFile << std::endl;
  std::cout << "Output: " << outputFile << std::endl;
  std::cout << "The length of the input text: " << T.size() << std::endl;
  std::cout << "The number of maximum substrings: " << maximumSubstringCount << std::endl;
  std::cout << "_________________________________" << std::endl;
  std::cout << "\033[39m" << std::endl;
}