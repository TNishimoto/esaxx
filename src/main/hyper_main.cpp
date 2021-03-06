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
#include "../hyper/rlcp_interval_enumerator.hpp"
#include "../rlbwt/io.hpp"
#include <sdsl/bit_vectors.hpp>
#include "../rlbwt/fpos_data_structure.hpp"

#include "../rlbwt/bwt_decompress.hpp"
#include "../rlbwt/light_rlbwt.hpp"

using CHAR = char;
using INDEX = uint64_t;
using LCPINTV = stool::LCPInterval<uint64_t>;

/*
void testLCPIntervals(std::string inputFile, bool lightWeight, bool correctCheck)
{
    stool::rlbwt::RLBWT<std::vector<CHAR>, std::vector<INDEX>> rlestr = stool::rlbwt::Constructor::load_RLBWT_from_file<CHAR, INDEX>(inputFile);
    using FPOSDS = std::vector<uint64_t>;
    using RDS = stool::lcp_on_rlbwt::RLBWTDataStructures<RLBWT<>, uint64_t, FPOSDS>;
    FPOSDS fposds = stool::lcp_on_rlbwt::FPosDataStructure::construct(rlestr);
    RDS ds = RDS(rlestr, fposds);

    auto testVec = stool::lcp_on_rlbwt::HyperSetConstructor<RLBWT<>, RDS>::constructLCPIntervals(rlestr, &ds);

    if (correctCheck)
    {

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
*/

/*
void test2(std::string inputFile, bool lightWeight)
{

    using RLBWT_STR = stool::rlbwt::RLBWT<std::vector<CHAR>, std::vector<INDEX>>;
    RLBWT_STR rlestr = stool::rlbwt::Constructor::load_RLBWT_from_file<CHAR, INDEX>(inputFile);
    lcp_on_rlbwt::RLBWTDataStructures<RLBWT_STR, uint64_t> rlbwtds(rlestr, lightWeight);
    lcp_on_rlbwt::RLCPIntervalEnumerator<RLBWT_STR, uint64_t, RDS> enumerator(&rlbwtds);

    for(auto w : enumerator){
        w.first.print();
    }
}
*/

void testMaximalSubstrings(std::string inputFile, string mode)
{

    sdsl::int_vector<> diff_char_vec;
    std::vector<bool> run_bits;
    stool::rlbwt2::load_RLBWT_from_file(inputFile, diff_char_vec, run_bits);
    sdsl::wt_huff<> wt;
    construct_im(wt, diff_char_vec);

    //uint64_t input_text_size = ds.str_size();
    std::vector<stool::LCPInterval<uint64_t>> test_Intervals;

    char LPOSMODE = mode[0];
    char FPOSMODE = mode[1];

    std::cout << "LPOS Data Structure: " << LPOSMODE << std::endl;
    std::cout << "FPOS Data Structure: " << FPOSMODE << std::endl;

    if (LPOSMODE == '0')
    {
        std::vector<uint64_t> lpos_vec = stool::rlbwt2::construct_lpos_array(run_bits);
        assert(diff_char_vec.size() + 1 == lpos_vec.size());
        using LPOSDS = std::vector<uint64_t>;

        if (FPOSMODE == '0')
        {
            using FPOSDS = std::vector<uint64_t>;
            using RDS = stool::lcp_on_rlbwt::RLBWTDataStructures<uint64_t, LPOSDS, FPOSDS>;
            FPOSDS fposds = stool::lcp_on_rlbwt::FPosDataStructure::construct(diff_char_vec, lpos_vec);
            RDS ds = RDS(diff_char_vec, wt, lpos_vec, fposds);
            std::vector<stool::LCPInterval<uint64_t>> tmp = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::computeMaximalSubstrings(&ds);
            test_Intervals.swap(tmp);
        }
        else
        {
            using FPOSDS = stool::lcp_on_rlbwt::LightFPosDataStructure;
            using RDS = stool::lcp_on_rlbwt::RLBWTDataStructures<uint64_t, LPOSDS, FPOSDS>;
            FPOSDS fposds = stool::lcp_on_rlbwt::LightFPosDataStructure(diff_char_vec, lpos_vec, wt);
            RDS ds = RDS(diff_char_vec, wt, lpos_vec, fposds);
            std::vector<stool::LCPInterval<uint64_t>> tmp = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::computeMaximalSubstrings(&ds);
            test_Intervals.swap(tmp);
        }
    }
    else
    {
        stool::EliasFanoVector lpos_vec;
        lpos_vec.build_from_bit_vector(run_bits);
        using LPOSDS = stool::EliasFanoVector;
        if (FPOSMODE == '0')
        {
            using FPOSDS = std::vector<uint64_t>;
            using RDS = stool::lcp_on_rlbwt::RLBWTDataStructures<uint64_t, LPOSDS, FPOSDS>;
            FPOSDS fposds = stool::lcp_on_rlbwt::FPosDataStructure::construct(diff_char_vec, lpos_vec);
            RDS ds = RDS(diff_char_vec, wt, lpos_vec, fposds);
            std::vector<stool::LCPInterval<uint64_t>> tmp = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::computeMaximalSubstrings(&ds);
            test_Intervals.swap(tmp);
        }
        else
        {
            using FPOSDS = stool::lcp_on_rlbwt::LightFPosDataStructure;
            using RDS = stool::lcp_on_rlbwt::RLBWTDataStructures<uint64_t, LPOSDS, FPOSDS>;
            FPOSDS fposds = stool::lcp_on_rlbwt::LightFPosDataStructure(diff_char_vec, lpos_vec, wt);
            RDS ds = RDS(diff_char_vec, wt, lpos_vec, fposds);
            std::vector<stool::LCPInterval<uint64_t>> tmp = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::computeMaximalSubstrings(&ds);
            test_Intervals.swap(tmp);
        }
    }
    /*
    if (lightWeight == 0)
    {
    }
    else if (lightWeight == 1)
    {
        std::cout << "LightWeight Mode" << std::endl;

        std::vector<stool::LCPInterval<uint64_t>> tmp = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::computeMaximalSubstrings(&ds);
        test_Intervals.swap(tmp);
    }
    else
    {
        std::cout << "LightWeight2 Mode" << std::endl;

        stool::EliasFanoVector efv;
        efv.build_from_bit_vector(run_bits);

        std::vector<stool::LCPInterval<uint64_t>> tmp = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::computeMaximalSubstrings(&ds);
        test_Intervals.swap(tmp);
    }
    */

    /*
    if (input_text_size - 10 < UINT32_MAX)
    {
    }
    else
    {
        std::vector<stool::LCPInterval<uint64_t>> tmp = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::computeMaximalSubstrings(&ds);
        test_Intervals.swap(tmp);
    }
    */

    std::vector<char> text = stool::bwt::decompress_bwt(inputFile);
    vector<INDEX> sa = stool::construct_suffix_array(text);
    using BWT = stool::esaxx::ForwardBWT<char, std::vector<char>, std::vector<INDEX>>;
    BWT bwt(&text, &sa);
    vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_maximal_substrings<char, INDEX>(text, sa);
    stool::beller::equal_check_lcp_intervals(test_Intervals, correct_intervals);
    std::cout << "OK!" << std::endl;
}

void computeMaximalSubstrings(std::string inputFile, std::string outputFile, string mode)
{
    auto start = std::chrono::system_clock::now();

    sdsl::int_vector<> diff_char_vec;
    std::vector<bool> run_bits;
    stool::rlbwt2::load_RLBWT_from_file(inputFile, diff_char_vec, run_bits);
    sdsl::wt_huff<> wt;
    construct_im(wt, diff_char_vec);

    //uint64_t input_text_size = ds.str_size();
    std::vector<stool::LCPInterval<uint64_t>> test_Intervals;

    char LPOSMODE = mode[0];
    char FPOSMODE = mode[1];

    std::cout << "LPOS Data Structure: " << LPOSMODE << std::endl;
    std::cout << "FPOS Data Structure: " << FPOSMODE << std::endl;

    std::ofstream out(outputFile, std::ios::out | std::ios::binary);
    if (!out)
    {
        throw std::runtime_error("Cannot open the output file!");
    }
    uint64_t input_text_size = 0;
    uint64_t ms_count = 0;

    if (LPOSMODE == '0')
    {
        std::vector<uint64_t> lpos_vec = stool::rlbwt2::construct_lpos_array(run_bits);
        assert(diff_char_vec.size() + 1 == lpos_vec.size());
        using LPOSDS = std::vector<uint64_t>;

        if (FPOSMODE == '0')
        {
            using FPOSDS = std::vector<uint64_t>;
            using RDS = stool::lcp_on_rlbwt::RLBWTDataStructures<uint64_t, LPOSDS, FPOSDS>;
            FPOSDS fposds = stool::lcp_on_rlbwt::FPosDataStructure::construct(diff_char_vec, lpos_vec);
            RDS ds = RDS(diff_char_vec, wt, lpos_vec, fposds);

            input_text_size = ds.str_size();
            std::cout << "Enumerate Maximal Substrings..." << std::endl;
            ms_count = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::outputMaximalSubstrings(out, &ds);
        }
        else
        {
            using FPOSDS = stool::lcp_on_rlbwt::LightFPosDataStructure;
            using RDS = stool::lcp_on_rlbwt::RLBWTDataStructures<uint64_t, LPOSDS, FPOSDS>;
            FPOSDS fposds = stool::lcp_on_rlbwt::LightFPosDataStructure(diff_char_vec, lpos_vec, wt);
            RDS ds = RDS(diff_char_vec, wt, lpos_vec, fposds);

            input_text_size = ds.str_size();
            std::cout << "Enumerate Maximal Substrings..." << std::endl;
            ms_count = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::outputMaximalSubstrings(out, &ds);
        }
    }
    else
    {
        stool::EliasFanoVector lpos_vec;
        lpos_vec.build_from_bit_vector(run_bits);
        using LPOSDS = stool::EliasFanoVector;
        if (FPOSMODE == '0')
        {
            using FPOSDS = std::vector<uint64_t>;
            using RDS = stool::lcp_on_rlbwt::RLBWTDataStructures<uint64_t, LPOSDS, FPOSDS>;
            FPOSDS fposds = stool::lcp_on_rlbwt::FPosDataStructure::construct(diff_char_vec, lpos_vec);
            RDS ds = RDS(diff_char_vec, wt, lpos_vec, fposds);

            input_text_size = ds.str_size();
            std::cout << "Enumerate Maximal Substrings..." << std::endl;
            ms_count = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::outputMaximalSubstrings(out, &ds);
        }
        else
        {
            using FPOSDS = stool::lcp_on_rlbwt::LightFPosDataStructure;
            using RDS = stool::lcp_on_rlbwt::RLBWTDataStructures<uint64_t, LPOSDS, FPOSDS>;
            FPOSDS fposds = stool::lcp_on_rlbwt::LightFPosDataStructure(diff_char_vec, lpos_vec, wt);
            RDS ds = RDS(diff_char_vec, wt, lpos_vec, fposds);

            input_text_size = ds.str_size();
            std::cout << "Enumerate Maximal Substrings..." << std::endl;
            ms_count = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::outputMaximalSubstrings(out, &ds);
        }
    }

    /*
    std::cout << "Enumerate Maximal Substrings..." << std::endl;
    uint64_t ms_count = 0;
    if (input_text_size - 10 < UINT32_MAX)
    {
        ms_count = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::outputMaximalSubstrings(out, &ds);
    }
    else
    {
        ms_count = stool::lcp_on_rlbwt::HyperSetConstructor<RDS>::outputMaximalSubstrings(out, &ds);
    }
    */
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    double bps = ((double)input_text_size / ((double)elapsed / 1000)) / 1000;

    std::cout << "\033[31m";
    std::cout << "______________________RESULT______________________" << std::endl;
    std::cout << "RLBWT File \t\t\t\t\t : " << inputFile << std::endl;    
    std::cout << "Output \t\t\t\t\t : " << outputFile << std::endl;
    std::cout << "LPOS Vector \t\t\t\t\t : " << (LPOSMODE == '0' ? "std::vector<uint64_t>" : "EliasFano") << std::endl;
    std::cout << "FPOS Vector \t\t\t\t\t : " << (FPOSMODE == '0' ? "std::vector<uint64_t>" : "EliasFano") << std::endl;
    std::cout << "The length of the input text \t\t : " << input_text_size << std::endl;
    std::cout << "The number of maximum substrings \t : " << ms_count << std::endl;
    std::cout << "Excecution time \t\t\t : " << elapsed << "[ms]" << std::endl;
    std::cout << "Character per second \t\t\t : " << bps << "[KB/s]" << std::endl;

    std::cout << "_______________________________________________________" << std::endl;
    std::cout << "\033[39m" << std::endl;
}

int main(int argc, char *argv[])
{
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    //p.add<bool>("mode", 'm', "mode", false, false);
    p.add<string>("output_file", 'o', "output file name", false, "");
    p.add<string>("mode", 'm', "mode", false, "00");
    p.add<bool>("debug", 'd', "debug", false, false);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    bool debug = p.get<bool>("debug");
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

    if (!debug)
    {
        computeMaximalSubstrings(inputFile, outputFile, mode);
    }
    else
    {
        testMaximalSubstrings(inputFile, mode);

        //test2(inputFile, mode);
    }
}
