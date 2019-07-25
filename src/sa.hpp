#pragma once
#include <unordered_set>
#include <cassert>
#include "divsufsort.h"
#include "divsufsort64.h"


namespace stool
{

template <typename CHAR = uint8_t,typename INDEX = uint64_t>
std::vector<INDEX> constructSA(std::vector<CHAR> &text);
//template std::vector<uint64_t> constructSA<char,uint64_t>(std::vector<char>&);


template <typename CHAR = char,typename INDEX = uint64_t>
std::vector<INDEX> constructISA(std::vector<CHAR> &text, std::vector<INDEX> &sa);

template <typename CHAR = char,typename INDEX = uint64_t>
std::vector<INDEX> constructLCP(std::vector<CHAR> &text, std::vector<INDEX> &sa, std::vector<INDEX> &isa);
template <typename CHAR = char,typename INDEX = uint64_t>
std::vector<INDEX> constructLCP(std::vector<CHAR> &text, std::vector<INDEX> &sa);

template <typename CHAR,typename INDEX>
std::vector<CHAR> constructBWT(std::vector<CHAR> &text, std::vector<INDEX> &sa);


} // namespace stool