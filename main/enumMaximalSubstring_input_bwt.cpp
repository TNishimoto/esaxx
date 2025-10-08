
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/lcp_dac.hpp>
#include <sdsl/lcp_support_sada.hpp>

#include "stool/include/light_stool.hpp"

#include "../include/common.hpp"
#include "libdivsufsort/sa.hpp"
#include "../include/postorder_maximal_substring_intervals.hpp"
#include "../include/forward_bwt.hpp"
#include "../include/test/bwt_decompress.hpp"

using namespace std;
using CHAR = uint8_t;
using INDEX = uint32_t;
uint64_t input_text_size = 0;
std::vector<std::pair<std::string, uint64_t>> execution_time_messages;

std::vector<CHAR> decompress_bwt2(std::vector<uint8_t> &bwt, std::vector<INDEX> &lcpArray)
{
  std::vector<INDEX> sa;
  std::vector<INDEX> isa;

  std::vector<uint64_t> FreqArr, C;
  FreqArr.resize(256, 0);
  C.resize(256, 0);

  //int_vector<> iv;
  //iv.resize(bwt.size());

  uint8_t minChar = 255;
  //uint64_t minCharMinOcc = UINT64_MAX;
  std::vector<INDEX> selectArray;

  uint64_t minCharPos = 0;
  selectArray.resize(bwt.size());
  sa.resize(bwt.size());
  isa.resize(bwt.size());

  for (uint64_t i = 0; i < bwt.size(); i++)
  {
    //iv[i] = bwt[i];
    selectArray[i] = FreqArr[bwt[i]];
    if (bwt[i] < minChar)
    {
      minChar = bwt[i];
      minCharPos = i;
    }
    FreqArr[bwt[i]]++;
  }

  for (uint64_t i = 1; i < FreqArr.size(); i++)
  {
    C[i] = C[i - 1] + FreqArr[i - 1];
  }
  //wt_gmr<> wt;
  //construct_im(wt, iv);

  std::vector<CHAR> outputText;
  int64_t x = bwt.size() - 1;
  std::vector<bool> checker;
  checker.resize(bwt.size(), false);
  outputText.resize(bwt.size(), 0);
  uint64_t p = minCharPos;

  while (x >= 0)
  {
    uint64_t v = (x +1) == (int64_t)bwt.size() ? 0 : x+1;
    sa[p] = v;
    isa[sa[p]] = p;
    outputText[x] = bwt[p];
    if (checker[p])
    {
      std::cout << "Error!" << std::endl;
      throw -1;
    }
    else
    {
      checker[p] = true;
    }
    x--;
    p = selectArray[p] + C[bwt[p]];
  }


  std::vector<INDEX> tmplcpArray = stool::ArrayConstructor::construct_LCP_array<CHAR, INDEX>(outputText, sa, isa);
  lcpArray.swap(tmplcpArray);
  return outputText;
}

uint64_t iterateMS(string filename, std::ofstream &out)
{
  // Load BWT
  std::vector<uint8_t> bwt2;
  stool::bwt::load(filename, bwt2);
  input_text_size = bwt2.size();

  // Construction Suffix Array
  std::vector<INDEX> lcpArray;

  auto start_sa = std::chrono::system_clock::now();
  std::vector<CHAR> T = decompress_bwt2(bwt2, lcpArray);

  //std::vector<uint8_t> bwt2;
  /*
  std::vector<CHAR> outputText;
  std::vector<INDEX> outputSA;
  stool::bwt::decompress_bwt(bwt2, outputText, outputSA);
  std::vector<INDEX> sa2 = stool::construct_suffix_array(outputText);
  std::vector<INDEX> lcpArray2 = stool::constructLCP<CHAR, INDEX>(outputText, sa2);
  */

  

  auto end_sa = std::chrono::system_clock::now();
  double sa_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_sa - start_sa).count();
  execution_time_messages.push_back(std::pair<std::string, uint64_t>("SA construction time\t\t", sa_construction_time));

  // Construction LCP Array
  //auto start_lcp = std::chrono::system_clock::now();
  //std::vector<INDEX> lcpArray = stool::constructLCP<CHAR, INDEX>(T, sa);
  //auto end_lcp = std::chrono::system_clock::now();
  //double lcp_array_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_lcp - start_lcp).count();
  //execution_time_messages.push_back(std::pair<std::string, uint64_t>("LCP array construction time\t", lcp_array_construction_time));

  // Enumerating maximal substring
  auto start_ms = std::chrono::system_clock::now();
  stool::esaxx::PostorderMaximalSubstringIntervals<CHAR, INDEX, std::vector<INDEX>, std::vector<uint8_t>> pmsi;
  pmsi.construct(&lcpArray, &bwt2);

  uint64_t count = 0;
  for (auto it : pmsi)
  {
    out.write(reinterpret_cast<const char *>(&it), sizeof(stool::LCPInterval<INDEX>));
    ++count;
  }
  std::cout << "Peek stack size: " << pmsi.compute_peek_stack_size() << std::endl;

  auto end_ms = std::chrono::system_clock::now();

  double ms_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_ms - start_ms).count();
  execution_time_messages.push_back(std::pair<std::string, uint64_t>("MS Construction time\t\t", ms_construction_time));
  return count;
}

int main(int argc, char *argv[])
{

  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);
  p.add<string>("output_file", 'o', "output file name", false, "");
  //p.add<bool>("print", 'p', "print info", false, true);
  //p.add<string>("format", 'f', "output format (binary or csv)", false, "binary");
  //p.add<string>("mode", 'm', "mode(esaxx or succinct)", false, "esaxx");
  //p.add<bool>("memory", 'u', "using only main memory (0 or 1)", false, 1);

  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");
  string outputFile = p.get<string>("output_file");
  //string format = p.get<string>("format");
  string format = "binary";

  //string mode = p.get<string>("mode");
  //bool usingMemory = p.get<bool>("memory");

  if (format != "binary")
  {
    format = "csv";
  }

  //bool isPrint = p.get<bool>("print");

  if (outputFile.size() == 0)
  {
    if (format == "csv")
    {
      outputFile = inputFile + ".max.csv";
    }
    else
    {
      outputFile = inputFile + ".max";
    }
  }

  auto start = std::chrono::system_clock::now();

  std::ofstream out(outputFile, std::ios::out | std::ios::binary);
  if (!out)
  {
    throw std::runtime_error("Cannot open the output file!");
  }
  uint64_t ms_count = 0;
  std::vector<stool::LCPInterval<INDEX>> intervals;

  //mode = "non-compressed";
  ms_count = iterateMS(inputFile, out);

  auto end = std::chrono::system_clock::now();
  double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "\033[31m";
  std::cout << "______________________RESULT______________________" << std::endl;
  //std::cout << "mode \t\t\t\t\t : " << mode << std::endl;
  std::cout << "File \t\t\t\t\t : " << inputFile << std::endl;
  std::cout << "Output \t\t\t\t\t : " << outputFile << std::endl;
  std::cout << "Output format \t\t\t\t : " << format << std::endl;
  std::cout << "The length of the input text \t\t : " << input_text_size << std::endl;
  //if(mode == "sdsl"){
  //  std::cout << "Using only main memory \t\t\t : " << (usingMemory ? "true" : "false") << std::endl;
  //}
  //std::cout << "The number of maximum substrings: " << maximumSubstringCount << std::endl;
  std::cout << "The number of maximum substrings \t : " << ms_count << std::endl;
  std::cout << "Excecution time \t\t\t : " << elapsed << "[ms]" << std::endl;
  for (auto it : execution_time_messages)
  {
    std::cout << "|\t " << it.first << " : " << it.second << "[ms]" << std::endl;
  }

  std::cout << "_______________________________________________________" << std::endl;
  std::cout << "\033[39m" << std::endl;
}