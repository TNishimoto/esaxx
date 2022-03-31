// License: MIT http://opensource.org/licenses/MIT
/*
  This code was copied from https://takeda25.hatenablog.jp/entry/20101202/1291269994 and I modified it.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "stool/include/cmdline.h"
#include "stool/include/debug.hpp"
#include "stool/include/io.hpp"
#include "../main/common.hpp"
#include "../test/naive_algorithms.hpp"
#include "../src/minimal_substrings/minimal_substring_iterator.hpp"

/*
#include "../../module/rlbwt_iterator/src/include/rlbwt_iterator.hpp"
#include "../test/old_postorder_maximal_substrings.hpp"
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/lcp_dac.hpp>
#include <sdsl/lcp_support_sada.hpp>
#include "../postorder_suffix_tree_intervals.hpp"
#include "../postorder_maximal_substring_intervals.hpp"
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/lcp_dac.hpp>
#include <sdsl/lcp_support_sada.hpp>
#include "../forward_bwt.hpp"
*/
using namespace std;
using CHAR = uint8_t;
using INDEX = uint64_t;

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

/*
std::vector<char> to_char_string(std::vector<uint8_t> &text){

}
*/

void minimal_substring_test(std::vector<uint8_t> &text)
{
  /*
  bool is_contained_minus_character = stool::esaxx::check_test(text);
  if (is_contained_minus_character)
  {
    std::cout << "This text contains minus character!" << std::endl;
  }
  */
  std::vector<INDEX> sa = stool::esaxx::construct_naive_SA_with_uint64<uint8_t, INDEX>(text);
  std::vector<INDEX> lcpArray = stool::constructLCP<uint8_t, INDEX>(text, sa);
  vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_minimal_substrings_with_uint64<uint8_t, INDEX>(text, sa);
  vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::compute_minimal_substrings(text, sa, lcpArray);
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
}

void minimal_substring_test(std::string filename)
{
  std::cout << "minimal substring intervals test";

  vector<char> text;
  stool::IO::load(filename, text); // input text
  // vector<char> text = stool::load_char_vec_from_file(filename, true); // input text
  // std::vector<INDEX> sa = stool::construct_naive_SA<char, INDEX>(text);
  std::vector<INDEX> sa = stool::esaxx::construct_naive_SA_with_uint64<char, INDEX>(text);
  std::vector<INDEX> lcpArray = stool::constructLCP<char, INDEX>(text, sa);
  vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_minimal_substrings_with_uint64<char, INDEX>(text, sa);
  vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::compute_minimal_substrings(text, sa, lcpArray);
  stool::sort_in_preorder(test_intervals);

  //;
  // = stool::esaxx::MinimalSubstringIntervals<>::compute_minimal_substrings<>(text, sa, lcpArray);

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

  /*

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
  using LCP_RLBWT = stool::rlbwt::ForwardLCPArray<std::vector<INDEX>>;
  using SA_RLBWT = stool::rlbwt::ForwardSA<std::vector<INDEX>>;
  stool::rlbwt::RLBWT<std::vector<char>, std::vector<INDEX>> rlestr;
  stool::rlbwt::Constructor::construct_from_file<>(rlestr, filename);
  LCP_RLBWT lcpArrayOnRLBWT;
  lcpArrayOnRLBWT.construct_from_rlbwt(&rlestr, false);
  SA_RLBWT *sa_pointer = const_cast<SA_RLBWT *>(lcpArrayOnRLBWT.get_ForwardSA());
  vector<stool::LCPInterval<INDEX>> rlbwt_test_intervals = stool::esaxx::compute_minimal_substrings<char, INDEX, SA_RLBWT, LCP_RLBWT>(text, *sa_pointer, lcpArrayOnRLBWT);
  stool::sort_in_preorder(rlbwt_test_intervals);
  stool::equal_check(correct_intervals, rlbwt_test_intervals);

  std::cout << "[RLBWT OK!]" << std::endl;
  */
}

int main(int argc, char *argv[])
{
  for (uint64_t i = 0; i < 100; i++)
  {
    std::vector<uint8_t> text = stool::StringCreator::create_uint8_t_binary_string(10, i);
    minimal_substring_test(text);
  }

  /*
  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);

  p.parse_check(argc, argv);
  string filename = p.get<string>("input_file");

  vector<char> text;
  stool::IO::load(filename, text); // input text
  text.push_back(0);
  bool is_contained_minus_character = stool::esaxx::check_test(text);
  if (is_contained_minus_character)
  {
    std::cout << "This text contains minus character!" << std::endl;
  }

  minimal_substring_test("test");
  */

  /*
using SA_PLAIN = std::vector<INDEX>;
using SA_SDSL = sdsl::csa_sada<>;
using LCP_PLAIN = std::vector<INDEX>;
using LCPINTERVALS = vector<stool::LCPInterval<INDEX>>;
using LCP_SDSL = sdsl::lcp_dac<>;
using LCP_RLBWT = stool::rlbwt::ForwardLCPArray<std::vector<INDEX>>;
using SA_RLBWT = stool::rlbwt::ForwardSA<std::vector<INDEX>>;
using BWT_RLBWT = stool::rlbwt::ForwardBWT<std::vector<char>, std::vector<INDEX>>;

stool::rlbwt::RLBWT<std::vector<char>, std::vector<INDEX>> rlestr;
stool::rlbwt::Constructor::construct_from_file<>(rlestr, filename);
LCP_RLBWT lcp_rlbwt;
lcp_rlbwt.construct_from_rlbwt(&rlestr, false);

SA_PLAIN sa_naive = stool::esaxx::construct_naive_SA_with_uint64<char, INDEX>(text);
LCP_PLAIN lcp_naive = stool::constructLCP<char, INDEX>(text, sa_naive);

SA_SDSL sa_sdsl;
construct(sa_sdsl, filename, 1);
LCP_SDSL lcp_sdsl;
construct(lcp_sdsl, filename, 1);

SA_RLBWT *sa_pointer = const_cast<SA_RLBWT *>(lcp_rlbwt.get_ForwardSA());

BWT_RLBWT bwt_rlbwt(&rlestr);
using BWT_SDSL = decltype(sa_sdsl.bwt);

lcp_interval_test(text, sa_naive, sa_naive, lcp_naive, "ESAXX");
lcp_interval_test(text, sa_naive, sa_sdsl, lcp_sdsl, "SDSL");
lcp_interval_test(text, sa_naive, *sa_pointer, lcp_rlbwt, "RLBWT");

maximal_interval_test(text, sa_naive, sa_sdsl, lcp_sdsl, sa_sdsl.bwt, "SDSL");
maximal_interval_test(text, sa_naive, *sa_pointer, lcp_sdsl, bwt_rlbwt, "RLBWT");
  */
}