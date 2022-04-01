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

using namespace std;
using CHAR = uint8_t;
using INDEX = uint64_t;

void minimal_substring_test(std::vector<uint8_t> &text)
{
  std::vector<INDEX> sa = stool::esaxx::construct_naive_SA_with_uint64<uint8_t, INDEX>(text);
  std::vector<INDEX> lcpArray = stool::constructLCP<uint8_t, INDEX>(text, sa);
  vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_minimal_substrings_with_uint64<uint8_t, INDEX>(text, sa);
  vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::compute_minimal_substrings(text, sa, lcpArray, true);
  stool::sort_in_preorder(test_intervals);

  stool::equal_check(correct_intervals, test_intervals);
}

int main(int argc, char *argv[])
{
  std::cout << "Test computing minimal substrings using deterministic binary strings" << std::endl;
  for (uint64_t x = 0; x < 4; x++)
  {
    uint64_t len = std::pow(10, (x + 1));
    std::cout << " \t Test, length = " << len << ": " << std::flush;
    for (uint64_t i = 0; i < 100; i++)
    {
      std::vector<uint8_t> text = stool::StringGenerator::create_uint8_t_binary_string(len, i);
      text.push_back(0);
      minimal_substring_test(text);
      std::cout << "+" << std::flush;
    }
    std::cout << std::endl;
  }


  std::cout << "Test computing minimal substrings using random binary strings" << std::endl;
  for (uint64_t i = 0 ; i < 1000; i++)
  {
    std::vector<uint8_t> text = stool::StringGenerator::create_uint8_t_binary_string_of_random_length(100);
    text.push_back(0);
    minimal_substring_test(text);
    std::cout << "+" << std::flush;
  }
  std::cout << std::endl;
  


  std::cout << "Test computing minimal substrings using deterministic 8-ary strings" << std::endl;
  for (uint64_t x = 0; x < 4; x++)
  {
    uint64_t len = std::pow(10, (x + 1));
    std::cout << " \t Test, length = " << len << ": " << std::flush;
    for (uint64_t i = 0; i < 100; i++)
    {
      std::vector<uint8_t> text = stool::StringGenerator::create_uint8_t_8_ary_string(len, i);
      text.push_back(0);
      minimal_substring_test(text);
      std::cout << "+" << std::flush;
    }
    std::cout << std::endl;
  }

  std::cout << "Test computing minimal substrings using random 8-ary strings" << std::endl;
  for (uint64_t i = 0 ; i < 1000; i++)
  {
    std::vector<uint8_t> text = stool::StringGenerator::create_uint8_t_8_ary_string_of_random_length(100);
    text.push_back(0);
    minimal_substring_test(text);
    std::cout << "+" << std::flush;
  }
  std::cout << std::endl;
}