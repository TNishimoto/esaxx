#pragma once
#include <iostream>
#include <string>
#include <memory>

//#include "fmindex.hpp"
#include "stool/src/io.hpp"
#include "stool/src/sa_bwt_lcp.hpp"

#include "minimal_substring_iterator.hpp"

namespace stool
{
namespace esaxx
{
// The minimal substring tree of a string T is the trie for minimal substrings for T.
template <typename CHAR = uint8_t, typename INDEX = uint64_t>
class MinimalSubstringTree
{
private:
  // Construct the outputParents using lcp intervals of minimal substring for T.
  static std::vector<INDEX> constructMSIntervalParents(std::vector<LCPInterval<INDEX>> &intervals)
  {
    std::vector<INDEX> outputParents;
    std::stack<INDEX> stack;
    outputParents.resize(intervals.size(), std::numeric_limits<INDEX>::max());
    for (INDEX i = 0; i < intervals.size(); i++)
    {
      if (i % 100000 == 0)
        std::cout << "\r"
                  << "Computing minimal substrings... :[4/4][" << i << "/" << intervals.size() << "]" << std::flush;
      LCPInterval<INDEX> &interval = intervals[i];

      while (stack.size() > 0)
      {
        LCPInterval<INDEX> &parentInterval = intervals[stack.top()];

        if (parentInterval.i <= interval.i && interval.j <= parentInterval.j)
        {
          break;
        }
        else
        {
          stack.pop();
        }
      }
      if (stack.size() > 0)
      {
        outputParents[i] = stack.top();
      }
      stack.push(i);
    }
    std::cout << "[END]" << std::endl;
    return outputParents;
  }

public:
  std::vector<LCPInterval<INDEX>> nodes;
  std::vector<INDEX> parents;
  // The outputIntervals stores lcp intervals of minimal substring.
  // The lcp intervals sorted in lexcographically order.
  // The i-th element in the outputParents stores the index of the parent of the node representing the i-th minimal substring.

  static void construct(std::vector<CHAR> &text, std::vector<LCPInterval<INDEX>> &outputIntervals, std::vector<INDEX> &outputParents)
  {
    std::cout << "Constructing Suffix Array" << std::endl;
    std::vector<INDEX> sa = stool::constructSA<CHAR, INDEX>(text);
    std::cout << "Constructing LCP Array" << std::endl;
    std::vector<INDEX> lcpArray = stool::constructLCP<CHAR, INDEX>(text, sa);
    std::cout << "Constructing BWT" << std::endl;
    std::vector<CHAR> bwt = stool::constructBWT<CHAR, INDEX>(text, sa);

    std::cout << "Constructing Minimal Substrings" << std::endl;
    std::vector<stool::LCPInterval<INDEX>> msVec = stool::esaxx::MinimalSubstringIterator<CHAR, INDEX>::constructSortedMinimalSubstringsWithoutSpecialMarker(bwt, sa, lcpArray);
    outputIntervals.swap(msVec);
    std::cout << "Constructing Minimal Substring Tree" << std::endl;
    std::vector<INDEX> tmp = MinimalSubstringTree::constructMSIntervalParents(outputIntervals);
    outputParents.swap(tmp);
    //text.pop_back();
  }
  void construct(std::string &text)
  {
    MinimalSubstringTree::construct(text, this->nodes, this->parents);
  }
  void write(std::string filepath, std::vector<CHAR> &text)
  {

    std::ofstream out(filepath, std::ios::out | std::ios::binary);
    //variables.serialize(out);
    //std::hash<std::vector<CHAR>> hash_fn;
    //INDEX hash = hash_fn(text);
    INDEX hash = 0;

    INDEX len = text.size();
    out.write(reinterpret_cast<const char *>(&hash), sizeof(uint64_t));
    out.write(reinterpret_cast<const char *>(&len), sizeof(uint64_t));
    INDEX nodeVecSize = nodes.size();
    out.write(reinterpret_cast<const char *>(&nodeVecSize), sizeof(INDEX));
    out.write(reinterpret_cast<const char *>(&nodes[0]), nodeVecSize * sizeof(LCPInterval<INDEX>));

    //stool::IO::write(out, nodes, false);
    INDEX parentVecSize = parents.size();
    out.write(reinterpret_cast<const char *>(&parentVecSize), sizeof(INDEX));
    out.write(reinterpret_cast<const char *>(&parents[0]), parentVecSize * sizeof(INDEX));
    //stool::IO::write(out, parents, false);

    out.close();
    //this->printInfo();
  }
  void load(std::string filepath, std::vector<CHAR> &text)
  {
    std::ifstream m_ifs(filepath);
    bool mSubstrFileExist = m_ifs.is_open();
    if (!mSubstrFileExist)
    {
      std::cout << "\033[31m";
      std::cout << "Error: " << filepath << "does not exist." << std::endl;
      std::cout << "\033[39m" << std::endl;
      std::exit(EXIT_FAILURE);
    }

    std::ifstream inputStream;
    inputStream.open(filepath, std::ios::binary);

    //std::hash<std::string> hash_fn;
    //INDEX hash = hash_fn(text);
    INDEX hash = 0;

    INDEX len = text.size();

    INDEX load_hash;
    INDEX load_len;

    inputStream.read((char *)&load_hash, sizeof(INDEX));
    inputStream.read((char *)&load_len, sizeof(INDEX));
    //stool::IO::load(inputStream, nodes, false);
    //stool::IO::load(inputStream, parents, false);

    INDEX nodeVecSize;
    INDEX parentVecSize;

    inputStream.read((char *)(&nodeVecSize), sizeof(INDEX));
    nodes.resize(nodeVecSize);
    inputStream.read((char *)&(nodes)[0], nodeVecSize * sizeof(LCPInterval<INDEX>));

    inputStream.read((char *)(&parentVecSize), sizeof(INDEX));
    parents.resize(parentVecSize);
    inputStream.read((char *)&(parents)[0], parentVecSize * sizeof(INDEX));

    inputStream.close();
    if (load_hash != hash || load_len != len)
    {
      std::cout << "\033[31m";
      std::cout << "Error: " << filepath << " is not the msub file of the input file." << std::endl;
      std::cout << "Please construct the msub file by msubstr.out.";
      std::cout << "\033[39m" << std::endl;
      std::exit(EXIT_FAILURE);
    }
    //this->printInfo();
  }
  void loadOrConstruct(std::string filepath, std::vector<CHAR> *text)
  {
    std::ifstream m_ifs(filepath);
    bool mSubstrFileExist = m_ifs.is_open();
    if (!mSubstrFileExist)
    {
      MinimalSubstringTree::construct(*text, this->nodes, this->parents);
      this->write(filepath, *text);
      //IO::write(filepath, this->nodes);
      //IO::write(filepath2, this->parents);
    }
    else
    {
      this->load(filepath, *text);
      //IO::load<LCPInterval>(filepath, this->nodes);
      //IO::load<uint64_t>(filepath2, this->parents);
    }
  }
};
} // namespace exaxx
} // namespace stool
