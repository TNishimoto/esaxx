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
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/lcp_dac.hpp>
#include <sdsl/lcp_support_sada.hpp>
#include "../postorder_suffix_tree_template.hpp"

using namespace std;
using CHAR = uint8_t;
using INDEXTYPE = int64_t;

std::vector<stool::LCPInterval<uint64_t>> iterateLCPIntervalsWithSDSL(std::string filename){
    sdsl::csa_sada<> csa;
    construct(csa, filename, 1);

    sdsl::lcp_dac<> dac;
    construct(dac, filename, 1);

    std::vector<stool::LCPInterval<uint64_t>> r;
    stool::esaxx::GPostorderSuffixTree<uint64_t, sdsl::csa_sada<>, sdsl::lcp_dac<>> gst;
    gst.set(std::move(csa), std::move(dac) );
    for(auto it : gst){
      r.push_back(it);
    }
    /*
    while(!gst.isEnded()){
      auto it = *gst;
      r.push_back(it);
      //std::cout << it.to_string() << std::endl;
      ++gst;
    }
    */
    return r;
}
std::vector<stool::LCPInterval<uint64_t>> iterateMSIntervalsWithSDSL(std::string filename){
    sdsl::csa_sada<> csa;
    construct(csa, filename, 1);

    sdsl::lcp_dac<> dac;
    construct(dac, filename, 1);

    using CHAR = uint8_t;
    vector<CHAR> T = stool::load_text_from_file(filename, true); // input text

    stool::DistinctCharacterCheckerOnInterval dcc;
    dcc.construct<CHAR, vector<CHAR>, sdsl::csa_sada<>>(T, csa);
  
    using ST = stool::esaxx::GPostorderSuffixTree<uint64_t, sdsl::csa_sada<>, sdsl::lcp_dac<>>;
    using STIT = stool::esaxx::GPostorderSTIterator<uint64_t, sdsl::csa_sada<>, sdsl::lcp_dac<>>;

    ST gst;
    gst.set(std::move(csa), std::move(dac) );
    STIT beg = gst.begin();
    STIT end = gst.end();

    stool::PostorderMaximalSubstringIterator<uint64_t, STIT, stool::DistinctCharacterCheckerOnInterval> ms_begin(beg, dcc);
    stool::PostorderMaximalSubstringIterator<uint64_t, STIT, stool::DistinctCharacterCheckerOnInterval> ms_end(end, dcc);
    
    std::vector<stool::LCPInterval<uint64_t>> r;
    while(ms_begin != ms_end ){
      auto it = *ms_begin;
      r.push_back(it);
      ++ms_begin;
    }
    return r;
}

std::vector<stool::LCPInterval<uint64_t>> iterateLCPIntervals(std::string filename){
    using INDEX = uint64_t;
    using CHAR = uint8_t;
    vector<CHAR> T = stool::load_text_from_file(filename, true); // input text
    std::cout << "Constructing Suffix Array" << std::endl;
    std::vector<INDEX> sa = stool::construct_suffix_array(T);
    std::cout << "Constructing LCP Array" << std::endl;
    std::vector<INDEX> lcpArray = stool::constructLCP<CHAR, INDEX>(T, sa);
    std::cout << "Constructing PST" << std::endl;

    stool::esaxx::PostorderSuffixTree<> pst(std::move(sa), std::move(lcpArray) );
    std::vector<stool::LCPInterval<uint64_t>> r;

    for(auto p = pst.begin(); p != pst.end();++p){
      r.push_back(*p);      
    }
    
    return r;
}

std::vector<stool::LCPInterval<int64_t>> iterateMSIntervals(std::string filename){
    using INDEX = int64_t;
    //using CHAR = uint8_t;

  vector<char> T = stool::load_text(filename); // input text
    //vector<char> T = stool::load_text_from_file(filename, true); // input text
    std::cout << "Constructing Suffix Array" << std::endl;
    std::vector<INDEX> sa;
    std::cout << "end" << std::endl;
    
    stool::PostorderMaximalSubstrings<INDEX> iterator = stool::PostorderMaximalSubstrings<INDEX>::construct(T, sa);
    std::vector<stool::LCPInterval<INDEX>> r;
    for (auto it : iterator)
    {
      r.push_back(it);
    }
        
    return r;
}


int main(int argc, char *argv[])
{

  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);
  //p.add<string>("output_file", 'o', "output file name", false, "");
  //p.add<bool>("print", 'p', "print info", false, true);
  //p.add<string>("format", 'f', "output format (binary or csv)", false, "binary");

  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");
  /*
  vector<char> T = stool::load_text(inputFile); // input text
  std::string s;  
  T[T.size()-1] = 100;
  for(auto& it : T){
    s.push_back(it);
    std::cout << (int)it << " ";
  }
  std::cout << std::endl;
  */

   std::cout << "SDSL version" << std::endl;
  auto vec = iterateLCPIntervalsWithSDSL(inputFile);
   std::cout << "Our version" << std::endl;
  auto vec2 = iterateLCPIntervals(inputFile);
   std::cout << "End" << std::endl;

  std::cout << vec.size() << "/" << vec2.size() << std::endl;
  for(int i=0;i<vec.size();i++){
    if(vec[i] != vec2[i]){
      throw -1;
    }
  }


  auto msVec = iterateMSIntervalsWithSDSL(inputFile);
  std::cout << msVec.size() << std::endl;

  auto msVec2 = iterateMSIntervals(inputFile);
  std::cout << msVec2.size() << std::endl;


  /*
  for(auto& it: vec){
    std::cout << it.to_string() << std::endl;
  }

  for(auto& it: vec2){
    std::cout << it.to_string() << std::endl;
  }
  */

  /*
    sdsl::csa_sada<> csa;
    construct(csa, inputFile, 1);
    cout << "csa.size(): " << csa.size() << endl;
    cout << "csa.sigma : " << csa.sigma << endl;
    cout << csa << endl;
    cout << extract(csa, 0, csa.size()-1) << endl;

    sdsl::lcp_dac<> dac;
    construct(dac, inputFile, 1);
    cout << dac << std::endl;

    for(auto p = csa.begin(); p != csa.end();++p){
      std::cout << *p << " ";
    }

    for(auto p = dac.begin(); p != dac.end();++p){
      std::cout << *p << " ";
    }

    stool::esaxx::GPostorderSTIterator<uint64_t, sdsl::csa_sada<>, sdsl::lcp_dac<>> gst(&csa, &dac, true);

    while(!gst.isEnded()){
      auto it = *gst;
      std::cout << it.to_string() << std::endl;
      ++gst;
    }
  */
    /*
    sdsl::lcp_support_sada<> lcp_sada;
    construct(lcp_sada, inputFile, 1);
    cout << lcp_sada << std::endl;
    */

}