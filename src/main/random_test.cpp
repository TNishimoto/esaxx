#include <iostream>
#include <string>
#include <memory>
#include "stool/src/print.hpp"
#include "stool/src/cmdline.h"
#include "stool/src/io.hpp"
#include "stool/src/debug.hpp"
#include "../src/minimal_substrings/naive_minimal_substrings.hpp"
#include "../src/minimal_substrings/minimal_substring_iterator.hpp"

using namespace std;
using INDEX = uint64_t;
template <typename T>
void lcp_interval_test(vector<T> &text){
    stool::Printer::print(text);
    vector<stool::LCPInterval<INDEX>> correct_intervals = stool::esaxx::naive_compute_lcp_intervals<T, INDEX>(text);

    vector<stool::LCPInterval<INDEX>> test_intervals = stool::esaxx::compute_preorder_lcp_intervals<T, INDEX>(text);
    

    for(auto& p : correct_intervals){
        std::cout << p.to_string() << std::endl;
    }
    std::cout << std::endl;


    for(auto& p : test_intervals){
        std::cout << p.to_string() << std::endl;
    }
    std::cout << std::endl;


    stool::equal_check(correct_intervals, test_intervals);


}


template <typename T>
void minimal_substrings_test(vector<T> &text){
    //stool::Printer::print("text",text);

    vector<uint64_t> sa = stool::construct_naive_SA<T, uint64_t>(text);    

    std::vector<stool::LCPInterval<uint64_t>> collect_ms = stool::esaxx::naive_compute_minimal_substrings(text, sa);


    std::vector<uint64_t> lcpArray = stool::constructLCP<T, uint64_t>(text, sa);
    std::vector<T> bwt = stool::constructBWT<T, uint64_t>(text, sa);

    vector<stool::LCPInterval<uint64_t>> test_ms = stool::esaxx::MinimalSubstringIterator<T, uint64_t, vector<uint64_t>>::constructSortedMinimalSubstrings(bwt, sa, lcpArray);
/*
for(auto& p : collect_ms){
        std::cout << p.to_string() << std::endl;
    }    
for(auto& p : test_ms){
        std::cout << p.to_string() << std::endl;
    }    
*/
    //std::vector<std::string> test_ms = stool::esaxx::naive_compute_minimal_substrings(text);

    //stool::Printer::print("collect",sa);
    //stool::Printer::print("test   ",sa2);    
    stool::equal_check(collect_ms, test_ms);
    //std::cout << "OK!" << std::endl;
}

void test(){
    std::vector<char> text{'m', 'i', 's', 'i', 's', 's', 'i', 'p', 'p', 'i', '$'};
    vector<uint64_t> sa = stool::construct_naive_SA<char, uint64_t>(text); 
    stool::Printer::print("SA", sa);
    auto intvVec = stool::esaxx::naive_compute_minimal_substrings(text, sa);
    for(auto& p : intvVec){
        std::cout << p.to_string() << std::endl;
    }    
}

int main(int argc, char *argv[]){
    
    cmdline::parser p;
    p.add<uint64_t>("size", 'l', "text length", true);
    p.parse_check(argc, argv);
    uint64_t size = p.get<uint64_t>("size");
    uint64_t loop = 10000;

    
    
    for(size_t i=0;i<loop;i++){
        if(i %100 == 0) std::cout << "+" << std::flush;
        std::vector<char> text = stool::create_deterministic_integers<char>(10, 10, 0, i);
        text.push_back(0);
        lcp_interval_test(text);
        
        
        //minimal_substrings_test(text);
        //suffix_array_test(text);
    }
    std::cout << std::endl;
    
    /*
    for(size_t i=0;i<loop;i++){
        if(i %100 == 0) std::cout << "+" << std::flush;
        std::vector<uint8_t> text = stool::create_deterministic_integers<uint8_t>(size, 255, 0, i);
        suffix_array_test(text);
    }
    std::cout << std::endl;

    for(size_t i=0;i<loop;i++){
        if(i %100 == 0) std::cout << "+" << std::flush;
        std::vector<int32_t> text = stool::create_deterministic_integers<int32_t>(size, 255, -255, i);
        suffix_array_test(text);
    }
    std::cout << std::endl;

    for(size_t i=0;i<loop;i++){
        if(i %100 == 0) std::cout << "+" << std::flush;
        std::vector<uint32_t> text = stool::create_deterministic_integers<uint32_t>(size, 510, 0, i);
        suffix_array_test(text);
    }
    std::cout << std::endl;

    for(size_t i=0;i<loop;i++){
        if(i %100 == 0) std::cout << "+" << std::flush;
        std::vector<int64_t> text = stool::create_deterministic_integers<int64_t>(size, 1024, -1024, i);
        suffix_array_test(text);
    }
    std::cout << std::endl;

    for(size_t i=0;i<loop;i++){
        if(i %100 == 0) std::cout << "+" << std::flush;
        std::vector<uint64_t> text = stool::create_deterministic_integers<uint64_t>(size, 2048, 0, i);
        suffix_array_test(text);
    }
    std::cout << std::endl;

    */
   std::cout << "END" << std::endl;

}