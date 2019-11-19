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
#include "../test/old_postorder_maximal_substrings.hpp"
#include "common.hpp"
#include "libdivsufsort/sa.hpp"
#include "../postorder_maximal_substring_intervals.hpp"
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/lcp_dac.hpp>
#include <sdsl/lcp_support_sada.hpp>
#include "../../module/rlbwt_iterator/src/include/rlbwt_iterator.hpp"
#include "../forward_bwt.hpp"
using namespace std;
using CHAR = char;
using INDEX = uint64_t;

/*
void iterateMSwithSDSL(){

}
*/

uint64_t input_text_size = 0;
/*
double sa_construction_time = 0;
double lcp_array_construction_time = 0;
double bwt_construction_time = 0;
double ms_construction_time = 0;
*/
std::vector<std::pair<std::string, uint64_t>> execution_time_messages;

uint64_t iterateMSWithRLBWT(string filename, std::ofstream &out){

  using LCP = stool::rlbwt::ForwardLCPArray<INDEX, std::vector<INDEX>>;
  using SA = stool::rlbwt::ForwardSA<INDEX, std::vector<INDEX>>;
  auto start_prep = std::chrono::system_clock::now();
  stool::rlbwt::RLBWT<CHAR, INDEX> rlestr;
  stool::rlbwt::Constructor::construct_from_file<CHAR, INDEX>(rlestr, filename);
  auto end_prep = std::chrono::system_clock::now();
  double prep_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_prep - start_prep).count();
  execution_time_messages.push_back(std::pair<std::string, uint64_t>("RLBWT construction time\t\t", prep_time));

  input_text_size = rlestr.str_size();

  auto start_lcp = std::chrono::system_clock::now();
  LCP lcpArray;
  lcpArray.construct_from_rlbwt(&rlestr, false);
  auto end_lcp = std::chrono::system_clock::now();
  double lcp_array_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_lcp - start_lcp).count();
  execution_time_messages.push_back(std::pair<std::string, uint64_t>("Sampling LCP & SA construction time\t", lcp_array_construction_time));
  
  
  const SA* sa_pointer = lcpArray.get_ForwardSA();

  using BWT_RLBWT = stool::rlbwt::ForwardBWT<CHAR, INDEX>;
  BWT_RLBWT bwt_rlbwt(&rlestr);
  
  auto start_ms = std::chrono::system_clock::now();
  stool::esaxx::PostorderMaximalSubstringIntervals<CHAR, INDEX, SA, LCP, BWT_RLBWT > pmsi;
  pmsi.construct(sa_pointer, &lcpArray, &bwt_rlbwt);
  uint64_t count = 0;
  for (auto it : pmsi)
  {
	  out.write(reinterpret_cast<const char *>(&it), sizeof(stool::LCPInterval<INDEX>));
    ++count;
  }
  auto end_ms = std::chrono::system_clock::now();
  double ms_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_ms - start_ms).count();
  execution_time_messages.push_back(std::pair<std::string, uint64_t>("MS Construction time\t\t\t", ms_construction_time));
  return count;  
}
uint64_t iterateMSWithSDSL(string filename, std::ofstream &out, bool usingMemory){


  std::string text;
  if(usingMemory){
    std::string tmp = stool::load_string_from_file(filename, false);
    text.swap(tmp);
  }
  auto start_sa = std::chrono::system_clock::now();
  sdsl::csa_sada<> sa;
  if(usingMemory){
    construct_im(sa, text, 1);
  }else{
    construct(sa, filename, 1);
  }
  auto end_sa = std::chrono::system_clock::now();
  auto sa_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_sa - start_sa).count();
  execution_time_messages.push_back(std::pair<std::string, uint64_t>("SA Construction time\t\t", sa_construction_time));

  input_text_size = sa.size();
  using BWT = decltype(sa.bwt);

  auto start_lcp = std::chrono::system_clock::now();
  sdsl::lcp_dac<> lcpArray;
  if(usingMemory){
    construct_im(lcpArray, text, 1);
  }else{
    construct(lcpArray, filename, 1);
  }
  text.resize(0);
  text.shrink_to_fit();

  //construct_im(lcpArray, filename, 1);
  auto end_lcp = std::chrono::system_clock::now();
  double lcp_array_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_lcp - start_lcp).count();
  execution_time_messages.push_back(std::pair<std::string, uint64_t>("LCP array construction time\t", lcp_array_construction_time));

  auto start_ms = std::chrono::system_clock::now();
  stool::esaxx::PostorderMaximalSubstringIntervals<CHAR, INDEX, sdsl::csa_sada<>, sdsl::lcp_dac<>, BWT> pmsi;
  pmsi.construct(&sa, &lcpArray, &sa.bwt);
  uint64_t count = 0;
  for (auto it : pmsi)
  {
	  out.write(reinterpret_cast<const char *>(&it), sizeof(stool::LCPInterval<INDEX>));
    ++count;
  }
  auto end_ms = std::chrono::system_clock::now();
  double ms_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_ms - start_ms).count();
  execution_time_messages.push_back(std::pair<std::string, uint64_t>("MS Construction time\t\t", ms_construction_time));
  return count;

}



uint64_t iterateMS(string filename, std::ofstream &out){
  vector<CHAR> T = stool::load_char_vec_from_file(filename, true); // input text
  input_text_size = T.size();

  auto start_sa = std::chrono::system_clock::now();
  std::vector<INDEX> sa = stool::construct_suffix_array(T);
  auto end_sa = std::chrono::system_clock::now();
  double sa_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_sa - start_sa).count();
  execution_time_messages.push_back(std::pair<std::string, uint64_t>("SA construction time\t\t", sa_construction_time));


  using BWT = stool::esaxx::ForwardBWT<CHAR, std::vector<CHAR>, std::vector<INDEX>>;
  BWT bwt(&T,&sa);

  /*
  auto start_bwt = std::chrono::system_clock::now();
  std::vector<CHAR> bwt = stool::esaxx::constructBWT<CHAR, INDEX, std::vector<INDEX>>(T, sa);
  auto end_bwt = std::chrono::system_clock::now();
  bwt_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_bwt - start_bwt).count();
  */

  auto start_lcp = std::chrono::system_clock::now();
  std::vector<INDEX> lcpArray = stool::constructLCP<CHAR, INDEX>(T, sa);
  auto end_lcp = std::chrono::system_clock::now();
  double lcp_array_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_lcp - start_lcp).count();
  execution_time_messages.push_back(std::pair<std::string, uint64_t>("LCP array construction time\t", lcp_array_construction_time));

  auto start_ms = std::chrono::system_clock::now();
  stool::esaxx::PostorderMaximalSubstringIntervals<CHAR, INDEX, std::vector<INDEX>, std::vector<INDEX>, BWT > pmsi;
  pmsi.construct(&sa, &lcpArray, &bwt);

    uint64_t count = 0;
  for (auto it : pmsi)
  {
	  out.write(reinterpret_cast<const char *>(&it), sizeof(stool::LCPInterval<INDEX>));
    ++count;
  }
  auto end_ms = std::chrono::system_clock::now();
  double ms_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_ms - start_ms).count();
  execution_time_messages.push_back(std::pair<std::string, uint64_t>("MS Construction time\t\t", ms_construction_time));
  return count;

}


uint64_t iterateMSwithOldESAXX(string filename, std::ofstream &out)
{
  vector<char> T = stool::load_text(filename);
  input_text_size = T.size();

  std::vector<int64_t> sa;
  stool::PostorderMaximalSubstrings<int64_t> pmsi = stool::PostorderMaximalSubstrings<int64_t>::construct(T, sa);
  uint64_t count = 0;
  for (auto it : pmsi)
  {
    auto tmp = stool::LCPInterval<INDEX>(it.i, it.j, it.lcp);
	  out.write(reinterpret_cast<const char *>(&tmp), sizeof(stool::LCPInterval<INDEX>));
        ++count;
  }
  return count;
}
int main(int argc, char *argv[])
{

  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);
  p.add<string>("output_file", 'o', "output file name", false, "");
  p.add<bool>("print", 'p', "print info", false, true);
  p.add<string>("format", 'f', "output format (binary or csv)", false, "binary");
  p.add<string>("mode", 'm', "mode(esaxx or succinct)", false, "esaxx");
  p.add<bool>("memory", 'u', "using only main memory (0 or 1)", false, 1);

  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");
  string outputFile = p.get<string>("output_file");
  string format = p.get<string>("format");
  string mode = p.get<string>("mode");
  bool usingMemory = p.get<bool>("memory");

  if (format != "binary")
  {
    format = "csv";
  }

  bool isPrint = p.get<bool>("print");

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
	if (!out){
    throw std::runtime_error("Cannot open the output file!");
  }
  uint64_t ms_count = 0;
  std::vector<stool::LCPInterval<INDEX>> intervals;
  if(mode == "old"){
    ms_count = iterateMSwithOldESAXX(inputFile, out);
  }else if(mode == "sdsl"){
    ms_count = iterateMSWithSDSL(inputFile, out, usingMemory);
  } else if(mode == "rlbwt"){
    ms_count = iterateMSWithRLBWT(inputFile, out);
  }
  else{
    mode = "non-compressed";
    ms_count = iterateMS(inputFile, out);
  }
  auto end = std::chrono::system_clock::now();
  double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  /*
  if (isPrint)
  {
    vector<CHAR> T = stool::load_char_vec_from_file(inputFile, true);
    std::vector<INDEX> sa = stool::construct_suffix_array(T);
    std::cout << "Maximal substrings in the file" << std::endl;
    stool::esaxx::print<CHAR, INDEX>(intervals, T, sa);
  }

  if (format != "binary")
  {
    vector<CHAR> T = stool::load_char_vec_from_file(inputFile, true);
    std::vector<INDEX> sa = stool::construct_suffix_array(T);
    stool::esaxx::writeText<CHAR, INDEX>(outputFile, intervals, T, sa);
  }
  else
  {
    stool::write_vector(outputFile, intervals, false);
  }
  */

  std::cout << "\033[31m";
  std::cout << "______________________RESULT______________________" << std::endl;
  std::cout << "mode \t\t\t\t\t : " << mode << std::endl; 
  std::cout << "File \t\t\t\t\t : " << inputFile << std::endl;
  std::cout << "Output \t\t\t\t\t : " << outputFile << std::endl;
  std::cout << "Output format \t\t\t\t : " << format << std::endl;  
  std::cout << "The length of the input text \t\t : " << input_text_size << std::endl;
  if(mode == "sdsl"){
    std::cout << "Using only main memory \t\t\t : " << (usingMemory ? "true" : "false") << std::endl;
  }
  //std::cout << "The number of maximum substrings: " << maximumSubstringCount << std::endl;
  std::cout << "The number of maximum substrings \t : " << ms_count << std::endl;
  std::cout << "Excecution time \t\t\t : " << elapsed << "[ms]" << std::endl;
  for(auto it : execution_time_messages){
  std::cout << "|\t " << it.first << " : " << it.second << "[ms]" << std::endl;

  }
  
  std::cout << "_______________________________________________________" << std::endl;
  std::cout << "\033[39m" << std::endl;
}