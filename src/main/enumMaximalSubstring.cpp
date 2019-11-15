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
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/lcp_dac.hpp>
#include <sdsl/lcp_support_sada.hpp>
#include "../../module/rlbwt_iterator/src/include/rlbwt_iterator.hpp"
using namespace std;
using CHAR = char;
using INDEXTYPE = uint64_t;

/*
void iterateMSwithSDSL(){

}
*/

uint64_t input_text_size = 0;
double sa_construction_time = 0;
double lcp_array_construction_time = 0;
double bwt_construction_time = 0;
double ms_construction_time = 0;

std::vector<stool::LCPInterval<INDEXTYPE>> iterateMSWithRLBWT(string filename){

  std::vector<stool::LCPInterval<INDEXTYPE>> intervals;

  //vector<CHAR> T = stool::load_char_vec_from_file(filename, true); // input text
  //input_text_size = T.size();
  vector<CHAR> T = stool::load_char_vec_from_file(filename, true); // input text
  input_text_size = T.size();

  using LCP = stool::rlbwt::ForwardLCPArray<INDEXTYPE, std::vector<INDEXTYPE>>;
  using SA = stool::rlbwt::ForwardSA<INDEXTYPE, std::vector<INDEXTYPE>>;
  stool::rlbwt::RLBWT<CHAR, INDEXTYPE> rlestr;
  stool::rlbwt::Constructor::construct_from_file<CHAR, INDEXTYPE>(rlestr, filename);


  LCP lcpArray;
  //lcpArray.to_lcp_array();
  lcpArray.construct_from_rlbwt(&rlestr, false);
  
  
  const SA* sa_pointer = lcpArray.get_ForwardSA();
  
  
  auto start_bwt = std::chrono::system_clock::now();
  std::vector<CHAR> bwt = stool::esaxx::constructBWT<CHAR, INDEXTYPE, SA >(T, *sa_pointer);
  auto end_bwt = std::chrono::system_clock::now();
  bwt_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_bwt - start_bwt).count();


  auto start_ms = std::chrono::system_clock::now();
  stool::PostorderMaximalSubstringIntervals<CHAR, INDEXTYPE, SA, LCP, std::vector<CHAR>> pmsi;
  pmsi.construct(sa_pointer, &lcpArray, &bwt);

  for (auto it : pmsi)
  {
    intervals.push_back(it);
  }
  auto end_ms = std::chrono::system_clock::now();
  ms_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_ms - start_ms).count();
  
  return intervals;
}
std::vector<stool::LCPInterval<INDEXTYPE>> iterateMSWithSDSL(string filename){
  vector<CHAR> T = stool::load_char_vec_from_file(filename, true); // input text
  input_text_size = T.size();


  std::cout << "SA" << std::endl;
  auto start_sa = std::chrono::system_clock::now();
  sdsl::csa_sada<> sa;
  construct(sa, filename, 1);
  auto end_sa = std::chrono::system_clock::now();
  sa_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_sa - start_sa).count();

  auto start_bwt = std::chrono::system_clock::now();
  std::vector<CHAR> bwt = stool::esaxx::constructBWT<CHAR, INDEXTYPE, sdsl::csa_sada<> >(T, sa);
  auto end_bwt = std::chrono::system_clock::now();
  bwt_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_bwt - start_bwt).count();

  std::cout << "LCP" << std::endl;
  auto start_lcp = std::chrono::system_clock::now();
  sdsl::lcp_dac<> lcpArray;
  construct(lcpArray, filename, 1);
  auto end_lcp = std::chrono::system_clock::now();
  lcp_array_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_lcp - start_lcp).count();

  auto start_ms = std::chrono::system_clock::now();
  stool::PostorderMaximalSubstringIntervals<CHAR, INDEXTYPE, sdsl::csa_sada<>, sdsl::lcp_dac<>, std::vector<CHAR>> pmsi;
  pmsi.construct(&sa, &lcpArray, &bwt);

  std::vector<stool::LCPInterval<INDEXTYPE>> intervals;
  for (auto it : pmsi)
  {
    intervals.push_back(it);
  }
  auto end_ms = std::chrono::system_clock::now();
  ms_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_ms - start_ms).count();

  return intervals;
}


std::vector<stool::LCPInterval<INDEXTYPE>> iterateMS(string filename){
  vector<CHAR> T = stool::load_char_vec_from_file(filename, true); // input text
  input_text_size = T.size();

  auto start_sa = std::chrono::system_clock::now();
  std::vector<INDEXTYPE> sa = stool::construct_suffix_array(T);
  auto end_sa = std::chrono::system_clock::now();
  sa_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_sa - start_sa).count();

  auto start_bwt = std::chrono::system_clock::now();
  std::vector<CHAR> bwt = stool::esaxx::constructBWT<CHAR, INDEXTYPE, std::vector<INDEXTYPE>>(T, sa);
  auto end_bwt = std::chrono::system_clock::now();
  bwt_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_bwt - start_bwt).count();


  auto start_lcp = std::chrono::system_clock::now();
  std::vector<INDEXTYPE> lcpArray = stool::constructLCP<CHAR, INDEXTYPE>(T, sa);
  auto end_lcp = std::chrono::system_clock::now();
  lcp_array_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_lcp - start_lcp).count();

  auto start_ms = std::chrono::system_clock::now();
  stool::PostorderMaximalSubstringIntervals<CHAR, INDEXTYPE, std::vector<INDEXTYPE>, std::vector<INDEXTYPE>, std::vector<CHAR>> pmsi;
  pmsi.construct(&sa, &lcpArray, &bwt);

  std::vector<stool::LCPInterval<INDEXTYPE>> intervals;
  for (auto it : pmsi)
  {
    intervals.push_back(it);
  }
  auto end_ms = std::chrono::system_clock::now();
  ms_construction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_ms - start_ms).count();

  return intervals;
}


std::vector<stool::LCPInterval<INDEXTYPE>> iterateMSwithOldESAXX(string filename)
{
  vector<char> T = stool::load_text(filename);
  input_text_size = T.size();

  std::vector<int64_t> sa;
  stool::PostorderMaximalSubstrings<int64_t> pmsi = stool::PostorderMaximalSubstrings<int64_t>::construct(T, sa);
  std::vector<stool::LCPInterval<INDEXTYPE>> intervals;
  for (auto it : pmsi)
  {
    intervals.push_back(stool::LCPInterval<INDEXTYPE>(it.i, it.j, it.lcp) );

  }
  return intervals;
}
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
  

  std::vector<stool::LCPInterval<INDEXTYPE>> intervals;
  if(mode == "old"){
    std::vector<stool::LCPInterval<INDEXTYPE>> tmp = iterateMSwithOldESAXX(inputFile);
    intervals.swap(tmp);
  }else if(mode == "sdsl"){
    std::vector<stool::LCPInterval<INDEXTYPE>> tmp = iterateMSWithSDSL(inputFile);
    intervals.swap(tmp);
  } else if(mode == "rlbwt"){

    std::vector<stool::LCPInterval<INDEXTYPE>> tmp = iterateMSWithRLBWT(inputFile);
    intervals.swap(tmp);
  }
  else{
    mode = "non-compressed";
    std::vector<stool::LCPInterval<INDEXTYPE>> tmp = iterateMS(inputFile);
    intervals.swap(tmp);
  }


  if (isPrint)
  {
    vector<CHAR> T = stool::load_char_vec_from_file(inputFile, true);
    std::vector<INDEXTYPE> sa = stool::construct_suffix_array(T);
    std::cout << "Maximal substrings in the file" << std::endl;
    stool::esaxx::print<CHAR, INDEXTYPE>(intervals, T, sa);
  }

  if (format != "binary")
  {
    vector<CHAR> T = stool::load_char_vec_from_file(inputFile, true);
    std::vector<INDEXTYPE> sa = stool::construct_suffix_array(T);
    stool::esaxx::writeText<CHAR, INDEXTYPE>(outputFile, intervals, T, sa);
  }
  else
  {
    stool::write_vector(outputFile, intervals, false);
  }
  auto end = std::chrono::system_clock::now();
  double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "\033[31m";
  std::cout << "______________________RESULT______________________" << std::endl;
  std::cout << "mode \t\t\t\t\t : " << mode << std::endl; 
  std::cout << "File \t\t\t\t\t : " << inputFile << std::endl;
  std::cout << "Output \t\t\t\t\t : " << outputFile << std::endl;
  std::cout << "Output format \t\t\t\t : " << format << std::endl;  
  std::cout << "The length of the input text \t\t : " << input_text_size << std::endl;
  //std::cout << "The number of maximum substrings: " << maximumSubstringCount << std::endl;
  std::cout << "The number of maximum substrings \t : " << intervals.size() << std::endl;
  std::cout << "Excecution time \t\t\t : " << elapsed << "[ms]" << std::endl;
  std::cout << "|\t SA Construction time \t\t : " << sa_construction_time << "[ms]" << std::endl;
  std::cout << "|\t LCP Array Construction time \t : " << lcp_array_construction_time << "[ms]" << std::endl;
  std::cout << "|\t BWT Construction time \t\t : " << bwt_construction_time << "[ms]" << std::endl;
  std::cout << "|\t MS Construction time \t\t : " << ms_construction_time << "[ms]" << std::endl;
  
  std::cout << "_______________________________________________________" << std::endl;
  std::cout << "\033[39m" << std::endl;
}