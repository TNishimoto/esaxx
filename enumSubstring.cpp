// License: MIT http://opensource.org/licenses/MIT
/*
  This code is copied from https://takeda25.hatenablog.jp/entry/20101202/1291269994 and I modified it.
*/

#include <iostream>
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

  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");

  vector<char> T;                 // input text
  readFile(inputFile.c_str(), T); // read file into T
  int32_t n = T.size();

  vector<int32_t> SA(n); // suffix array
  vector<int32_t> L(n);  // left boundaries of internal node
  vector<int32_t> R(n);  // right boundaries of internal node
  vector<int32_t> D(n);  // depths of internal node

  int32_t alphaSize = 0x100; // This can be very large
  int32_t nodeNum = 0;

  if (esaxx(T.begin(), SA.begin(),
            L.begin(), R.begin(), D.begin(),
            n, alphaSize, nodeNum) == -1)
  {
    return -1;
  }
  for (int i = 0; i < nodeNum; ++i)
  {
    int len = D[i];
    cout << R[i] - L[i] << "\t" << D[i] << "\t";
    int begin = SA[L[i]];
    for (int j = 0; j < len; ++j)
    {
      cout << T[begin + j];
    }
    cout << endl;
  }

  return 0;
}
