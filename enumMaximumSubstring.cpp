// License: MIT http://opensource.org/licenses/MIT
/*
  This code is copied from https://takeda25.hatenablog.jp/entry/20101202/1291269994 and I modified it.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "cmdline.h"
#include "esa.hxx"

using namespace std;

int readFile(const char *fn, vector<char> &T)
{
  FILE *fp = fopen(fn, "rb");
  if (fp == NULL)
  {
    cerr << "cannot open " << fn << endl;
    return -1;
  }

  if (fseek(fp, 0, SEEK_END) != 0)
  {
    cerr << "cannot fseek " << fn << endl;
    fclose(fp);
    return -1;
  }
  int n = ftell(fp);
  rewind(fp);
  if (n < 0)
  {
    cerr << "cannot ftell " << fn << endl;
    fclose(fp);
    return -1;
  }
  T.resize(n);
  if (fread(&T[0], sizeof(unsigned char), (size_t)n, fp) != (size_t)n)
  {
    cerr << "fread error " << fn << endl;
    fclose(fp);
    return -1;
  }

  fclose(fp);
  return 0;
}

int main(int argc, char *argv[])
{

  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);
  p.add<string>("output_file", 'o', "output file name", true);

  p.add<bool>("print", 'p', "print info", false, true);

  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");
  string outputFile = p.get<string>("output_file");
  bool isPrint = p.get<bool>("print");

  vector<char> T;                 // input text
  readFile(inputFile.c_str(), T); // read file into T
  int64_t n = T.size();

  vector<int64_t> SA(n); // suffix array
  vector<int64_t> L(n);  // left boundaries of internal node
  vector<int64_t> R(n);  // right boundaries of internal node
  vector<int64_t> D(n);  // depths of internal node

  int64_t alphaSize = 0x100; // This can be very large
  int64_t nodeNum = 0;

  if (esaxx(T.begin(), SA.begin(),
            L.begin(), R.begin(), D.begin(),
            n, alphaSize, nodeNum) == -1)
  {
    return -1;
  }

  int64_t size = T.size();

  vector<int64_t> rank(size);
  int64_t r = 0;
  for (int64_t i = 0; i < size; i++)
  {
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
              << "\t"
              << "occ"
              << "\t"
              << "SA[i]"
              << "\t"
              << "length"
              << "\t"
              << "string" << std::endl;
  }

  vector<stool::LCPInterval> buffer;
  ofstream os(outputFile, ios::out | ios::binary);
  if (!os)
    return 1;

  for (int64_t i = 0; i < nodeNum; ++i)
  {
    stool::LCPInterval interval(L[i], R[i], D[i]);
    int64_t len = D[i];
    if ((rank[interval.j - 1] - rank[interval.i] == 0))
    {
      continue;
    }
    buffer.push_back(interval);
    if (buffer.size() > 8192)
    {
      os.write((const char *)(&buffer[0]), sizeof(stool::LCPInterval) * buffer.size());
      buffer.clear();
    }

    if (isPrint)
    {
      cout << i << "\t" << interval.j - interval.i << "\t" << interval.i << "\t" << interval.lcp << "\t";
      int64_t begin = SA[interval.i];
      for (int64_t j = 0; j < len; ++j)
      {
        cout << T[begin + j];
      }
      cout << endl;
    }
  }
  os.write((const char *)(&buffer[0]), sizeof(stool::LCPInterval) * buffer.size());
  buffer.clear();
  os.close();
}