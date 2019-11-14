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

using namespace std;
using CHAR = uint8_t;
using INDEX = uint64_t;
#include "../esa.hxx"
#include "../postorder_maximal_substrings.hpp"
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


void lcp_interval_test(std::string filename)
{
    std::cout << "LCP interval test";

    vector<uint8_t> text = stool::load_text_from_file(filename, true); // input text
    std::vector<INDEX> sa = stool::construct_naive_SA<CHAR, INDEX>(text);
    vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_lcp_intervals<CHAR, INDEX>(text, sa);

    std::vector<INDEX> lcpArray = stool::constructLCP<CHAR, INDEX>(text, sa);

    vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::compute_preorder_lcp_intervals<CHAR, INDEX>(text, sa, lcpArray);


    sdsl::csa_sada<> csa;
    construct(csa, filename, 1);

    sdsl::lcp_dac<> dac;
    construct(dac, filename, 1);

    vector<stool::LCPInterval<INDEX>> sdsl_test_intervals = stool::esaxx::compute_preorder_lcp_intervals<CHAR, INDEX, sdsl::csa_sada<>, sdsl::lcp_dac<>>(text, csa, dac);

    stool::equal_check(correct_intervals, test_intervals);
    stool::equal_check(correct_intervals, sdsl_test_intervals);

    std::cout << "[OK!]" << std::endl;

}

void minimal_substring_test(std::string filename)
{
    std::cout << "minimal substring intervals test";

    vector<uint8_t> text = stool::load_text_from_file(filename, true); // input text
    std::vector<INDEX> sa = stool::construct_naive_SA<CHAR, INDEX>(text);
    std::vector<INDEX> lcpArray = stool::constructLCP<CHAR, INDEX>(text, sa);


    sdsl::csa_sada<> csa;
    construct(csa, filename, 1);

    sdsl::lcp_dac<> dac;
    construct(dac, filename, 1);


    vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_minimal_substrings<CHAR, INDEX>(text, sa);
    vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::compute_preorder_minimal_substrings<CHAR, INDEX>(text, sa, lcpArray);
    vector<stool::LCPInterval<INDEX>> sdsl_test_intervals = stool::esaxx::compute_preorder_minimal_substrings<CHAR, INDEX, sdsl::csa_sada<>, sdsl::lcp_dac<>>(text, csa, dac);


    stool::equal_check(correct_intervals, test_intervals);
    stool::equal_check(correct_intervals, sdsl_test_intervals);

    std::cout << "[OK!]" << std::endl;

}

void maximal_substring_test(std::string filename)
{
    std::cout << "maximal substring intervals test";

    vector<uint8_t> text = stool::load_text_from_file(filename, true); // input text
    std::vector<INDEX> sa = stool::construct_naive_SA<CHAR, INDEX>(text);
    std::vector<INDEX> lcpArray = stool::constructLCP<CHAR, INDEX>(text, sa);

    sdsl::csa_sada<> csa;
    construct(csa, filename, 1);

    sdsl::lcp_dac<> dac;
    construct(dac, filename, 1);


    vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_maximal_substrings<CHAR, INDEX>(text, sa);
    vector<stool::LCPInterval<INDEX>> test_intervals = stool::compute_preorder_maximal_substrings<CHAR, INDEX>(text, sa, lcpArray);
    vector<stool::LCPInterval<INDEX>> sdsl_test_intervals = stool::compute_preorder_maximal_substrings<CHAR, INDEX, sdsl::csa_sada<>, sdsl::lcp_dac<>>(text, csa, dac);
    /*
    for(auto it : correct_intervals){
      std::cout << it.to_string() << std::endl;      
    }
    std::cout << "end" << std::endl;
    for(auto it : test_intervals){
      std::cout << it.to_string() << std::endl;      
    }
    */


    stool::equal_check(correct_intervals, test_intervals);
    stool::equal_check(correct_intervals, sdsl_test_intervals);

    std::cout << "[OK!]" << std::endl;

}



int main(int argc, char *argv[])
{

  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);

  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");

  lcp_interval_test(inputFile);
  minimal_substring_test(inputFile);
  maximal_substring_test(inputFile);

}