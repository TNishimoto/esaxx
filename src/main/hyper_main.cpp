#include <cassert>
#include <chrono>
#include "../module/stool/src/io.hpp"
#include "../module/stool/src/sa_bwt_lcp.hpp"

#include "../module/stool/src/print.hpp"
#include "../module/stool/src/cmdline.h"
#include "../module/stool/src/debug.hpp"
#include "../module/libdivsufsort/sa.hpp"
#include "../module/rlbwt_iterator/src/include/rlbwt_iterator.hpp"
//#include "module/rlbwt_iterator/src/include/bwt.hpp"

#include "stool/src/io.hpp"
#include "stool/src/cmdline.h"
#include "stool/src/debug.hpp"

using namespace std;
using namespace stool;
using namespace stool::rlbwt;

//#include "hpp/bwt.hpp"
//#include "hpp/fmindex.hpp"
#include "../beller/beller_debug.hpp"
#include "../hyper/hyper_weiner.hpp"
#include "../main/common.hpp"
#include "../test/naive_algorithms.hpp"
#include "../postorder_maximal_substring_intervals.hpp"
#include "../forward_bwt.hpp"

using namespace std;
using namespace stool;
//using namespace stool::rlbwt;

using CHAR = char;
using INDEX = uint64_t;
using LCPINTV = stool::LCPInterval<uint64_t>;

void testLCPIntervals(std::string inputFile, bool correctCheck)
{

    stool::rlbwt::RLBWT<std::vector<CHAR>, std::vector<INDEX>> rlestr = stool::rlbwt::Constructor::load_RLBWT_from_file<CHAR, INDEX>(inputFile);

    auto testVec = stool::lcp_on_rlbwt::HyperSetConstructor<RLBWT<>>::constructLCPIntervals(rlestr);

    if (correctCheck)
    {
        /*
        if (text.size() <= 100)
        {
            std::cout << "Text: ";
            std::cout << text << std::endl;
        }
        */

        //RLBWT<>::check_text_for_rlbwt(text);

        std::vector<char> text2 = stool::load_char_vec_from_file(inputFile, true);
        vector<INDEX> sa = stool::construct_suffix_array(text2);
        auto correctLCP = stool::constructLCP(text2, sa);
        std::cout << "Correct" << std::endl;
        std::vector<LCPINTV> correct_intervals = stool::beller::naive_compute_complete_lcp_intervals<uint64_t>(sa, correctLCP);
        stool::beller::equal_check_lcp_intervals(testVec, correct_intervals);
        std::cout << "OK!" << std::endl;
    }
    std::cout << "rlbwt = " << rlestr.rle_size() << std::endl;
}

void testMaximalSubstrings(std::string inputFile)
{
    stool::rlbwt::RLBWT<std::vector<CHAR>, std::vector<INDEX>> rlestr = stool::rlbwt::Constructor::load_RLBWT_from_file<CHAR, INDEX>(inputFile);
    uint64_t input_text_size = rlestr.str_size();
    std::vector<stool::LCPInterval<uint64_t>> test_Intervals = stool::lcp_on_rlbwt::HyperSetConstructor<RLBWT<>>::computeMaximalSubstrings(rlestr);

    
    BackwardText<> backer;
    backer.construct_from_rlbwt(&rlestr, false);
    std::vector<char> text;
    text.resize(input_text_size);
    int64_t i = input_text_size-1;
    for(auto c : backer){
        text[i] = c;
        i--;
    }

    //std::vector<char> text = backer.to_vector();
    vector<INDEX> sa = stool::construct_suffix_array(text);
    
    using BWT = stool::esaxx::ForwardBWT<char, std::vector<char>, std::vector<INDEX>>;
    BWT bwt(&text, &sa);
    vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_maximal_substrings<char, INDEX>(text, sa);
    stool::beller::equal_check_lcp_intervals(test_Intervals, correct_intervals);
    std::cout << "OK!" << std::endl;
    
}

void computeMaximalSubstrings(std::string inputFile, std::string outputFile, bool correctCheck)
{
    auto start = std::chrono::system_clock::now();

    stool::rlbwt::RLBWT<std::vector<CHAR>, std::vector<INDEX>> rlestr = stool::rlbwt::Constructor::load_RLBWT_from_file<CHAR, INDEX>(inputFile);
    uint64_t input_text_size = rlestr.str_size();
    std::ofstream out(outputFile, std::ios::out | std::ios::binary);
    if (!out)
    {
        throw std::runtime_error("Cannot open the output file!");
    }
    std::cout << "Enumerate Maximal Substrings..." << std::endl;
    auto ms_count = stool::lcp_on_rlbwt::HyperSetConstructor<RLBWT<>>::outputMaximalSubstrings(rlestr, out);
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\033[31m";
    std::cout << "______________________RESULT______________________" << std::endl;
    std::cout << "RLBWT File \t\t\t\t\t : " << inputFile << std::endl;
    std::cout << "Output \t\t\t\t\t : " << outputFile << std::endl;
    std::cout << "The length of the input text \t\t : " << input_text_size << std::endl;
    std::cout << "The number of maximum substrings \t : " << ms_count << std::endl;
    std::cout << "Excecution time \t\t\t : " << elapsed << "[ms]" << std::endl;
    std::cout << "_______________________________________________________" << std::endl;
    std::cout << "\033[39m" << std::endl;
}
int main(int argc, char *argv[])
{
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("mode", 'm', "mode", false, "xx");
    p.add<string>("output_file", 'o', "output file name", false, "");

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string mode = p.get<string>("mode");
    string outputFile = p.get<string>("output_file");
    string format = "binary";

    std::ifstream ifs(inputFile);
    bool inputFileExist = ifs.is_open();
    if (!inputFileExist)
    {
        std::cout << inputFile << " cannot open." << std::endl;
        return -1;
    }

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

    computeMaximalSubstrings(inputFile, outputFile, true);
    //testMaximalSubstrings(inputFile);

}
