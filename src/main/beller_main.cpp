#include <cassert>
#include <chrono>
#include "../module/stool/src/io.hpp"
#include "../module/stool/src/sa_bwt_lcp.hpp"

#include "../module/stool/src/print.hpp"
#include "../module/stool/src/cmdline.h"
#include "../module/stool/src/debug.hpp"
#include "../module/libdivsufsort/sa.hpp"

//#include "hpp/bwt.hpp"
#include "../beller/fmindex.hpp"
#include "../beller/beller_interval.hpp"
#include "../beller/beller_debug.hpp"

using namespace std;
using namespace stool;
//using namespace stool::rlbwt;

using CHAR = char;
using INDEX = uint64_t;
using LCPINTV = stool::LCPInterval<uint64_t>;

void computeLCPIntervals(std::string inputFile, bool correctCheck)
{

  //string text = "";
  std::cout << "Loading : " << inputFile << std::endl;
  std::vector<char> text = stool::load_char_vec_from_file(inputFile, true);
  vector<INDEX> sa = stool::construct_suffix_array(text);
  sdsl::int_vector<> bwt;
  FMIndex::constructBWT(text, sa, bwt);

  std::vector<uint64_t> C;
  FMIndex::constructC(text, C);

  wt_huff<> wt;

  construct_im(wt, bwt);

  auto test_Intervals = stool::beller::computeLCPIntervals(bwt, C, wt);
  test_Intervals.push_back(LCPINTV(0, text.size()-1, 0));

  if (correctCheck)
  {
    auto correctLCP = stool::constructLCP(text, sa);
    std::cout << "Correct" << std::endl;
    std::vector<LCPINTV> correct_intervals = stool::beller::naive_compute_complete_lcp_intervals<uint64_t>(sa, correctLCP);
    stool::beller::equal_check_lcp_intervals(test_Intervals, correct_intervals);
    std::cout << "OK!" << std::endl;
  }
}

int main(int argc, char *argv[])
{
  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);
  p.add<string>("mode", 'm', "mode", false, "xx");

  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");
  string mode = p.get<string>("mode");

  std::ifstream ifs(inputFile);
  bool inputFileExist = ifs.is_open();
  if (!inputFileExist)
  {
    std::cout << inputFile << " cannot open." << std::endl;
    return -1;
  }
  computeLCPIntervals(inputFile, true);
}
