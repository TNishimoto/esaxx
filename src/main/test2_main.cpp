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
#include "../../module/rlbwt_iterator/src/include/rlbwt_iterator.hpp"

using namespace std;
using CHAR = uint8_t;
using INDEX = uint64_t;
#include "../test/old_postorder_maximal_substrings.hpp"
#include "common.hpp"
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/lcp_dac.hpp>
#include <sdsl/lcp_support_sada.hpp>
#include "../postorder_suffix_tree_intervals.hpp"
#include "../test/naive_algorithms.hpp"
#include "../postorder_maximal_substring_intervals.hpp"
#include "../src/minimal_substrings/minimal_substring_iterator.hpp"
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/lcp_dac.hpp>
#include <sdsl/lcp_support_sada.hpp>
#include "../forward_bwt.hpp"

template <typename CHAR = uint8_t>
bool compare_suffixes_with_uint64(const std::vector<CHAR> &text, const uint64_t x, const uint64_t y)
{
  uint64_t max = x < y ? text.size() - y : text.size() - x;
  for (uint64_t i = 0; i < max; i++)
  {
    uint64_t c1 = text[x + i];
    uint64_t c2 = text[y + i];
    if (c1 != c2)
    {
      return c1 < c2;
    }
  }
  return x > y;
}
template <typename CHAR = uint8_t, typename INDEX = uint64_t>
std::vector<INDEX> construct_naive_SA_with_uint64(const std::vector<CHAR> &text)
{
  std::vector<INDEX> r;
  for (uint64_t i = 0; i < text.size(); i++)
  {
    r.push_back(i);
  }

  std::sort(
      r.begin(),
      r.end(),
      [&](const uint64_t &x, const uint64_t &y) {
        return compare_suffixes_with_uint64(text, x, y);
      });
  return r;
}

bool check_test(vector<uint8_t> &text)
{
  for (auto &it : text)
  {
    if (it >= 128)
    {
      //throw std::logic_error("This text contains minus character!");
      return true;
    }
  }
  return false;
}
bool check_test(vector<char> &text)
{
  for (auto &it : text)
  {
    if (it < 0)
    {
      //throw std::logic_error("This text contains minus character!");
      return true;
    }
  }
  return false;
}
template <typename T, typename U>
bool super_equal_check(const T &vec1, const U &vec2)
{
  if (vec1.size() != vec2.size())
  {
    std::string s = std::string("String sizes are different!") + ", collect = " + std::to_string(vec1.size()) + ", test = " + std::to_string(vec2.size());

    throw std::logic_error(s);
  }
  for (uint64_t i = 0; i < vec1.size(); i++)
  {
    if (vec1[i] != vec2[i])
    {
      std::string msg = "collect_vec[" + std::to_string(i) + "] != test_vec[" + std::to_string(i) + "]";

      throw std::logic_error("Values are different! " + msg);
    }
  }
  return true;
}

void lcp_interval_test(std::string filename)
{
  std::cout << "LCP interval test";

  vector<char> text = stool::load_char_vec_from_file(filename, true); // input text
  //vector<uint8_t> text = stool::load_text_from_file(filename, true); // input text
  bool is_contained_minus_character = check_test(text);
  if (is_contained_minus_character)
  {
    std::cout << "This text contains minus character!" << std::endl;
  }

  std::vector<INDEX> sa = construct_naive_SA_with_uint64<char, INDEX>(text);
  vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_lcp_intervals<char, INDEX>(text, sa);
  std::vector<INDEX> lcpArray = stool::constructLCP<char, INDEX>(text, sa);
  vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::PostorderSuffixTreeIntervals<INDEX, std::vector<INDEX>, std::vector<INDEX>>::compute_lcp_intervals(sa, lcpArray);
  stool::sort_in_preorder(test_intervals);

  stool::equal_check(correct_intervals, test_intervals);
  std::cout << "[ESAXX OK!]" << std::endl;

  //vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::compute_preorder_lcp_intervals<CHAR, INDEX>(text, sa, lcpArray);

  //SDSL
  using SA_SDSL = sdsl::csa_sada<>;
  using LCP_SDSL = sdsl::lcp_dac<>;
  SA_SDSL csa;
  construct(csa, filename, 1);
  LCP_SDSL dac;
  construct(dac, filename, 1);
  //super_equal_check(sa, csa);
  //super_equal_check(lcpArray, dac);

  vector<stool::LCPInterval<INDEX>> sdsl_test_intervals = stool::esaxx::PostorderSuffixTreeIntervals<INDEX, SA_SDSL, LCP_SDSL>::compute_lcp_intervals(csa, dac);
  stool::sort_in_preorder(sdsl_test_intervals);
  stool::equal_check(correct_intervals, sdsl_test_intervals);
  std::cout << "[SDSL OK!]";

  // Postorder suffix interval tree with RLBWT
  //vector<char> char_text;
  //for(auto& it : text) char_text.push_back(it);
  using LCP_RLBWT = stool::rlbwt::ForwardLCPArray<INDEX, std::vector<INDEX>>;
  using SA_RLBWT = stool::rlbwt::ForwardSA<INDEX, std::vector<INDEX>>;
  stool::rlbwt::RLBWT<char, INDEX> rlestr;
  stool::rlbwt::Constructor::construct_from_file<char, INDEX>(rlestr, filename);
  LCP_RLBWT lcpArrayOnRLBWT;
  lcpArrayOnRLBWT.construct_from_rlbwt(&rlestr, false);
  SA_RLBWT *sa_pointer = const_cast<SA_RLBWT *>(lcpArrayOnRLBWT.get_ForwardSA());

  vector<stool::LCPInterval<INDEX>> rlbwt_test_intervals = stool::esaxx::PostorderSuffixTreeIntervals<INDEX, SA_RLBWT, LCP_RLBWT>::compute_lcp_intervals(*sa_pointer, lcpArrayOnRLBWT);
  stool::sort_in_preorder(rlbwt_test_intervals);
  //vector<stool::LCPInterval<INDEX>> rlbwt_test_intervals = stool::esaxx::compute_preorder_lcp_intervals<char, INDEX, SA, LCP >(char_text, *sa_pointer, lcpArrayOnRLBWT);
  stool::equal_check(correct_intervals, rlbwt_test_intervals);

  std::cout << "[RLBWT OK!]" << std::endl;
}

void minimal_substring_test(std::string filename)
{
  std::cout << "minimal substring intervals test";

  //vector<uint8_t> text = stool::load_text_from_file(filename, true); // input text
  vector<char> text = stool::load_char_vec_from_file(filename, true); // input text
  bool is_contained_minus_character = check_test(text);
  if (is_contained_minus_character)
  {
    std::cout << "This text contains minus character!" << std::endl;
  }
  //std::vector<INDEX> sa = stool::construct_naive_SA<char, INDEX>(text);
  std::vector<INDEX> sa = construct_naive_SA_with_uint64<char, INDEX>(text);
  std::vector<INDEX> lcpArray = stool::constructLCP<char, INDEX>(text, sa);
  vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_minimal_substrings_with_uint64<char, INDEX>(text, sa);
  vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::compute_minimal_substrings<char, INDEX>(text, sa, lcpArray);
  stool::sort_in_preorder(test_intervals);

  for (auto &it : correct_intervals)
  {
    std::cout << it.to_string() << std::endl;
  }
  std::cout << std::endl;
  for (auto &it : test_intervals)
  {
    std::cout << it.to_string() << std::endl;
  }

  stool::equal_check(correct_intervals, test_intervals);
  std::cout << "[ESAXX OK!]";

  if (!is_contained_minus_character)
  {
    using SA_SDSL = sdsl::csa_sada<>;
    using LCP_SDSL = sdsl::lcp_dac<>;
    SA_SDSL csa;
    construct(csa, filename, 1);
    LCP_SDSL dac;
    construct(dac, filename, 1);
    vector<stool::LCPInterval<INDEX>> sdsl_test_intervals = stool::esaxx::compute_minimal_substrings<char, INDEX, SA_SDSL, LCP_SDSL>(text, csa, dac);
    stool::sort_in_preorder(sdsl_test_intervals);
    stool::equal_check(correct_intervals, sdsl_test_intervals);
    std::cout << "[SDSL OK!]";
  }
  else
  {
    std::cout << "Skip SDSL check" << std::endl;
  }

  // Postorder suffix interval tree with RLBWT
  using LCP_RLBWT = stool::rlbwt::ForwardLCPArray<INDEX, std::vector<INDEX>>;
  using SA_RLBWT = stool::rlbwt::ForwardSA<INDEX, std::vector<INDEX>>;
  stool::rlbwt::RLBWT<char, INDEX> rlestr;
  stool::rlbwt::Constructor::construct_from_file<char, INDEX>(rlestr, filename);
  LCP_RLBWT lcpArrayOnRLBWT;
  lcpArrayOnRLBWT.construct_from_rlbwt(&rlestr, false);
  SA_RLBWT *sa_pointer = const_cast<SA_RLBWT *>(lcpArrayOnRLBWT.get_ForwardSA());
  vector<stool::LCPInterval<INDEX>> rlbwt_test_intervals = stool::esaxx::compute_minimal_substrings<char, INDEX, SA_RLBWT, LCP_RLBWT>(text, *sa_pointer, lcpArrayOnRLBWT);
  stool::sort_in_preorder(rlbwt_test_intervals);
  stool::equal_check(correct_intervals, rlbwt_test_intervals);

  std::cout << "[RLBWT OK!]" << std::endl;
}

void maximal_substring_test(std::string filename)
{
  std::cout << "maximal substring intervals test";

  //vector<uint8_t> text = stool::load_text_from_file(filename, true); // input text
  vector<char> text = stool::load_char_vec_from_file(filename, true); // input text
  bool is_contained_minus_character = check_test(text);
  if (is_contained_minus_character)
  {
    std::cout << "This text contains minus character!" << std::endl;
  }

  //std::vector<INDEX> sa = stool::construct_naive_SA<CHAR, INDEX>(text);
  std::vector<INDEX> sa = construct_naive_SA_with_uint64<char, INDEX>(text);
  std::vector<INDEX> lcpArray = stool::constructLCP<char, INDEX>(text, sa);
  using BWT = stool::esaxx::ForwardBWT<char, std::vector<char>, std::vector<INDEX>>;
  BWT bwt(&text,&sa);

  //std::vector<char> bwt = stool::esaxx::constructBWT<char, INDEX>(text, sa);
  vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_maximal_substrings<char, INDEX>(text, sa);
  vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::PostorderMaximalSubstringIntervals<char, INDEX, std::vector<INDEX>, std::vector<INDEX>, BWT >::compute_maximal_substrings(sa, lcpArray, bwt);
  stool::sort_in_preorder(test_intervals);

  stool::equal_check(correct_intervals, test_intervals);
  std::cout << "[ESAXX OK!]" << std::endl;

  using SA_SDSL = sdsl::csa_sada<>;
  using LCP_SDSL = sdsl::lcp_dac<>;
  SA_SDSL csa;
  construct(csa, filename, 1);
  LCP_SDSL dac;
  construct(dac, filename, 1);
  using BWT_SDSL = decltype(csa.bwt);

  vector<stool::LCPInterval<INDEX>> sdsl_test_intervals = stool::esaxx::PostorderMaximalSubstringIntervals<char, INDEX, SA_SDSL, LCP_SDSL, BWT_SDSL>::compute_maximal_substrings(csa, dac, csa.bwt);
  stool::sort_in_preorder(sdsl_test_intervals);
  stool::equal_check(correct_intervals, sdsl_test_intervals);
  std::cout << "[SDSL OK!]" << std::endl;

  // Postorder suffix interval tree with RLBWT
  vector<char> char_text;
  for (auto &it : text)
    char_text.push_back((char)it);
  std::vector<char> char_bwt = stool::esaxx::constructBWT<char, INDEX>(char_text, sa);
  using LCP_RLBWT = stool::rlbwt::ForwardLCPArray<INDEX, std::vector<INDEX>>;
  using SA_RLBWT = stool::rlbwt::ForwardSA<INDEX, std::vector<INDEX>>;
  stool::rlbwt::RLBWT<char, INDEX> rlestr;
  stool::rlbwt::Constructor::construct_from_file<char, INDEX>(rlestr, filename);
  LCP_RLBWT lcpArrayOnRLBWT;
  lcpArrayOnRLBWT.construct_from_rlbwt(&rlestr, false);
  SA_RLBWT *sa_pointer = const_cast<SA_RLBWT *>(lcpArrayOnRLBWT.get_ForwardSA());
  vector<stool::LCPInterval<INDEX>> rlbwt_test_intervals = stool::esaxx::PostorderMaximalSubstringIntervals<char, INDEX, SA_RLBWT, LCP_RLBWT, BWT >::compute_maximal_substrings(*sa_pointer, lcpArrayOnRLBWT, bwt);
  //vector<stool::LCPInterval<INDEX>> rlbwt_test_intervals = stool::esaxx::compute_minimal_substrings<char, INDEX, SA_RLBWT, LCP_RLBWT>(char_text, *sa_pointer, lcpArrayOnRLBWT);
  stool::sort_in_preorder(rlbwt_test_intervals);
  stool::equal_check(correct_intervals, rlbwt_test_intervals);

  std::cout << "[RLBWT OK!]" << std::endl;

  std::cout << "[OK!]" << std::endl;
}

int main(int argc, char *argv[])
{

  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);

  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");

  lcp_interval_test(inputFile);
  //minimal_substring_test(inputFile);
  maximal_substring_test(inputFile);
}