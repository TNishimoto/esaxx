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

template <typename T, typename U>
void lcp_interval_test(vector<char> &text, vector<INDEX> &correct_sa, T &test_sa, U &test_lcp, string test_name)
{
  std::cout << "LCP interval test: " << test_name << std::flush;
  vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_lcp_intervals<char, INDEX>(text, correct_sa);
  vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::PostorderSuffixTreeIntervals<INDEX, T, U >::compute_lcp_intervals(test_sa, test_lcp);
  stool::sort_in_preorder(test_intervals);
  stool::equal_check(correct_intervals, test_intervals);
  std::cout << "[OK!]" << std::endl;
}
template <typename T, typename U, typename BWT_TEST>
void maximal_interval_test(vector<char> &text, vector<INDEX> &correct_sa, T &test_sa, U &test_lcp, BWT_TEST &test_bwt, string test_name)
{

  std::cout << "Maximal substring interval test: " << test_name << std::flush;
  using BWT = stool::esaxx::ForwardBWT<char, std::vector<char>, std::vector<INDEX>>;
  BWT bwt(&text, &correct_sa);
  vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_maximal_substrings<char, INDEX>(text, correct_sa);

  vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::PostorderMaximalSubstringIntervals<char, INDEX, T, U, BWT_TEST>::compute_maximal_substrings(test_sa, test_lcp, test_bwt);
  stool::sort_in_preorder(test_intervals);

  stool::equal_check(correct_intervals, test_intervals);

  std::cout << "[OK!]" << std::endl;
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
    construct_im(csa, filename, 1);
    LCP_SDSL dac;
    construct_im(dac, filename, 1);
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

int main(int argc, char *argv[])
{

  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);

  p.parse_check(argc, argv);
  string filename = p.get<string>("input_file");

  //lcp_interval_test(inputFile);
  //minimal_substring_test(inputFile);
  //maximal_substring_test(inputFile);

  vector<char> text = stool::load_char_vec_from_file(filename, true); // input text
  bool is_contained_minus_character = check_test(text);
  if (is_contained_minus_character)
  {
    std::cout << "This text contains minus character!" << std::endl;
  }

  using SA_PLAIN = std::vector<INDEX>;
  using SA_SDSL = sdsl::csa_sada<>;
  using LCP_PLAIN = std::vector<INDEX>;
  using LCPINTERVALS = vector<stool::LCPInterval<INDEX>>;
  using LCP_SDSL = sdsl::lcp_dac<>;
  using LCP_RLBWT = stool::rlbwt::ForwardLCPArray<INDEX, std::vector<INDEX>>;
  using SA_RLBWT = stool::rlbwt::ForwardSA<INDEX, std::vector<INDEX>>;
  using BWT_RLBWT = stool::rlbwt::ForwardBWT<char, INDEX>;

  stool::rlbwt::RLBWT<char, INDEX> rlestr;
  stool::rlbwt::Constructor::construct_from_file<char, INDEX>(rlestr, filename);
  LCP_RLBWT lcp_rlbwt;
  lcp_rlbwt.construct_from_rlbwt(&rlestr, false);
  

  std::cout << "Suffix Array Test" << std::endl;

  SA_PLAIN sa_naive = construct_naive_SA_with_uint64<char, INDEX>(text);
  std::cout << "SDSL Test" << std::flush;
  SA_SDSL sa_sdsl;
  construct(sa_sdsl, filename, 1);
  super_equal_check(sa_naive, sa_sdsl);
  std::cout << "[OK!]" << std::endl;
  std::cout << "RLBWT Test" << std::flush;
  SA_RLBWT *sa_pointer = const_cast<SA_RLBWT *>(lcp_rlbwt.get_ForwardSA());
  std::vector<uint64_t> _sa_rlbwt;
  for(auto it : *sa_pointer) _sa_rlbwt.push_back(it);
  super_equal_check(sa_naive, _sa_rlbwt);
  std::cout << "[OK!]" << std::endl;

  std::cout << "LCP Array Test" << std::endl;
  std::cout << "SDSL Test" << std::flush;
  LCP_PLAIN lcp_naive = stool::constructLCP<char, INDEX>(text, sa_naive);
  LCP_SDSL lcp_sdsl;
  construct(lcp_sdsl, filename, 1);
  super_equal_check(lcp_naive, lcp_sdsl);
  std::cout << "[OK!]" << std::endl;
  std::cout << "RLBWT Test" << std::flush;
  std::vector<uint64_t> _lcp_rlbwt;
  for(auto it : lcp_rlbwt) _lcp_rlbwt.push_back(it);
  super_equal_check(lcp_naive, _lcp_rlbwt);
  std::cout << "[OK!]" << std::endl;

  BWT_RLBWT bwt_rlbwt(&rlestr);
  using BWT_SDSL = decltype(sa_sdsl.bwt);


  lcp_interval_test(text, sa_naive, sa_naive, lcp_naive, "ESAXX");
  lcp_interval_test(text, sa_naive, sa_sdsl, lcp_sdsl, "SDSL");
  lcp_interval_test(text, sa_naive, *sa_pointer, lcp_rlbwt, "RLBWT");

  maximal_interval_test(text, sa_naive, sa_sdsl, lcp_sdsl, sa_sdsl.bwt, "SDSL");
  maximal_interval_test(text, sa_naive, *sa_pointer, lcp_sdsl, bwt_rlbwt, "RLBWT");


  /*
  vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::PostorderSuffixTreeIntervals<INDEX, std::vector<INDEX>, std::vector<INDEX>>::compute_lcp_intervals(sa, lcpArray);
  stool::sort_in_preorder(test_intervals);
  stool::equal_check(correct_intervals, test_intervals);
  std::cout << "[ESAXX OK!]" << std::endl;

  LCPINTERVALS correct_intervals = stool::esaxx::naive_compute_lcp_intervals<char, INDEX>(text, sa_naive);
  */

}