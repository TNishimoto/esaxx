
#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "stool/src/io.hpp"
#include "stool/src/sa_bwt_lcp.hpp"

#include <stack>

namespace stool
{
namespace esaxx
{
template <typename INDEX = uint64_t>
void naive_compute_minimal_substrings(std::string &text, std::unordered_map<INDEX, INDEX> &previousPositionSet, INDEX strlen, std::vector<std::pair<INDEX, INDEX>> &output)
{
  if (strlen == 1)
  {
    std::unordered_map<char, INDEX> countMap;
    for (INDEX i = 0; i < text.size(); i++)
    {
      char c = text[i];
      auto it = countMap.find(c);
      if (it != countMap.end())
      {
        countMap[c]++;
      }
      else
      {
        countMap[c] = 1;
        output.push_back(std::pair<INDEX,INDEX>(i, 1) );

        //output.push_back(std::string(1, c));
      }
    }
    for (INDEX i = 0; i < text.size(); i++)
    {
      char c = text[i];
      INDEX count = countMap[c];
      if (count > 1)
      {
        previousPositionSet[i] = count;
      }
    }
  }
  else
  {
    std::unordered_map<std::string, INDEX> identifierMap;
    std::unordered_map<INDEX, std::pair<INDEX, uint8_t>> countMap;

    std::unordered_map<INDEX, INDEX> postionsToUniquePositions;

    std::unordered_map<INDEX, INDEX> nextPositionSet;

    for (auto p : previousPositionSet)
    {
      if (p.first != 0)
      {
        INDEX leftPosition = p.first - 1;
        std::string sub = text.substr(leftPosition, strlen);
        auto it = identifierMap.find(sub);
        if (it == identifierMap.end())
        {
          identifierMap[sub] = leftPosition;
          countMap[identifierMap[sub]] = std::pair<INDEX, uint8_t>(0, 0);
        }
        auto it2 = postionsToUniquePositions.find(leftPosition);
        if (it2 == postionsToUniquePositions.end())
        {
          postionsToUniquePositions[leftPosition] = identifierMap[sub];
          countMap[identifierMap[sub]] = std::pair<INDEX, uint8_t>(countMap[identifierMap[sub]].first + 1, 0);
        }
      }
      if (p.first != text.size() - 1)
      {
        INDEX rightPosition = p.first + 1;
        std::string sub = text.substr(rightPosition, strlen);
        auto it = identifierMap.find(sub);
        if (it == identifierMap.end())
        {
          identifierMap[sub] = rightPosition;
          countMap[identifierMap[sub]] = std::pair<INDEX, uint8_t>(0, 0);
        }
        auto it2 = postionsToUniquePositions.find(rightPosition);
        if (it2 == postionsToUniquePositions.end())
        {
          postionsToUniquePositions[rightPosition] = identifierMap[sub];
          countMap[identifierMap[sub]] = std::pair<INDEX, uint8_t>(countMap[identifierMap[sub]].first + 1, 0);
        }
      }
    }
    for (auto &p : postionsToUniquePositions)
    {
      INDEX pos = p.first;
      INDEX uniquePos = p.second;
      INDEX count = countMap[uniquePos].first;
      INDEX flag = countMap[uniquePos].second;

      if (count > 1)
      {
        nextPositionSet[pos] = count;
      }

      if (flag == 0)
      {
        if (count < previousPositionSet[uniquePos] && count < previousPositionSet[uniquePos + 1])
        {
          countMap[uniquePos] = std::pair<INDEX, uint8_t>(count, 1);
          std::string minimalSubstr = text.substr(pos, strlen);

          output.push_back(std::pair<INDEX,INDEX>(pos, strlen) );
        }
        else
        {
          countMap[uniquePos] = std::pair<INDEX, uint8_t>(count, 2);
        }
      }
    }
    previousPositionSet.clear();
    for (auto &p : nextPositionSet)
    {
      previousPositionSet[p.first] = p.second;
    }
  }
}

template <typename CHAR = char>
bool computeSubstr(const std::vector<CHAR> &T, const std::vector<CHAR> &pattern, uint64_t suf_pos, bool isBeg, bool isComp)
{
  uint64_t suf_len = T.size() - suf_pos;
  uint64_t min = pattern.size() < suf_len ? pattern.size() : suf_len;
  for (size_t i = 0; i < min; i++)
  {
    if (pattern[i] != T[suf_pos + i])
    {
      return isComp ? pattern[i] < T[suf_pos + i] : pattern[i] > T[suf_pos + i];
    }
  }
  uint64_t sufLastChar = suf_len > min ? 2 : 1; 
  uint64_t patternLastChar = pattern.size() > min ? 2 : isBeg ? 0 : 3; 

  return isComp ? patternLastChar < sufLastChar : patternLastChar > sufLastChar;
}

template <typename CHAR = char, typename INDEX = uint64_t>
stool::LCPInterval<INDEX> computeLCPInterval(const std::vector<CHAR> &T, const std::vector<CHAR> &pattern, const std::vector<INDEX> &sa)
{
  uint64_t p = T.size();
  auto beg = std::upper_bound(
      sa.begin(),
      sa.end(),
      p,
      [&](const INDEX &x, const INDEX &y) {
        if (x == T.size())
        {
          bool b = computeSubstr(T, pattern, y, true, true);
          return b;
        }
        else
        {
          bool b = computeSubstr(T, pattern, x, true, false);
          return b;
        }
      });

  auto end = std::upper_bound(
      sa.begin(),
      sa.end(),
      p,
      [&](const INDEX &x, const INDEX &y) {
        if (x == T.size())
        {
          bool b = computeSubstr(T, pattern, y, false, true);
          return b;
        }
        else
        {
          bool b = computeSubstr(T, pattern, y, false, false);
          return b;
        }
      });

  std::size_t begi = std::distance(sa.begin(), beg);
  std::size_t endi = std::distance(sa.begin(), end) - 1;

  return stool::LCPInterval<INDEX>(begi, endi, pattern.size());
}

template <typename INDEX = uint64_t>
std::vector<stool::LCPInterval<INDEX>> naive_compute_minimal_substrings(std::vector<char> &T, const std::vector<INDEX> &sa)
{
  std::string text(T.begin(), T.end());
  std::unordered_map<INDEX, INDEX> positionCountSet;
  std::vector<std::pair<INDEX, INDEX>> output;
  stool::esaxx::naive_compute_minimal_substrings<INDEX>(text, positionCountSet, 1, output);
  for (INDEX len = 2; len <= text.size(); len++)
  {
    //std::cout << "[" << len << "/" << positionCountSet.size() << "]\r" << std::flush;
    if (positionCountSet.size() == 0)
    {
      break;
    }
    else
    {
      stool::esaxx::naive_compute_minimal_substrings<INDEX>(text, positionCountSet, len, output);
    }
  }

  std::vector<stool::LCPInterval<INDEX>> r;
  r.push_back(stool::LCPInterval<INDEX>(0, text.size()-1,0 ));
  for(auto &it : output){
    std::vector<char> pattern;
    uint64_t pos = it.first;
    uint64_t len = it.second;
    for(uint64_t x=0;x<len;x++){
      pattern.push_back(T[pos+x]);
    }
    stool::LCPInterval<INDEX> intv = computeLCPInterval(T, pattern, sa);
    r.push_back(intv);
  }

    std::sort(
        r.begin(),
        r.end(),
        [](const stool::LCPInterval<INDEX> &x, const stool::LCPInterval<INDEX> &y) {
        if (x.i == y.i)
    {
      if (x.j == y.j)
      {
        return x.lcp < y.lcp;
      }
      else
      {
        return x.j > y.j;
      }
    }
    else
    {
      return x.i < y.i;
    }
      }
        );

  //std::cout << std::endl;
  //std::sort(output.begin(), output.end());
  return r;
}

} // namespace esaxx
} // namespace stool
