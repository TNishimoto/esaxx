
#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "../mycode.hpp"
#include <stack>

namespace stool
{
    /*
    template <typename INDEX = uint64_t>
    std::vector<std::string> naive_compute_minimal_substrings(vector<char> &T)
  {
    string text(T.begin(), T.end());
    std::unordered_map<std::string, INDEX> map;
    for (INDEX i = 0; i < text.size(); i++)
    {
      std::cout << "[" << i << "/" << text.size() << "]\r" << std::flush;
      for (INDEX j = i; j < text.size(); j++)
      {
        std::string sub = text.substr(i, 1 + j - i);
        auto it = map.find(sub);
        if (it != map.end())
        {
          map[sub]++;
        }
        else
        {
          map[sub] = 1;
        }
      }
    }
    std::cout << std::endl;
    std::vector<std::string> r;
    INDEX counter = 0;
    for (auto p : map)
    {
      std::cout << "[" << counter++ << "/" << map.size() << "]\r" << std::flush;

      std::string sub = p.first;
      if (sub.size() > 1)
      {
        std::string pref = sub.substr(0, sub.size() - 1);
        std::string suf = sub.substr(1);
        bool b1 = map[pref] != p.second;
        bool b2 = map[suf] != p.second;
        if (b1 && b2)
        {
          r.push_back(sub);
        }
      }
      else if (sub.size() == 0)
      {
        r.push_back(sub);
      }
    }
    std::cout << std::endl;
    std::sort(r.begin(), r.end());
    return r;
  }
  */
    template <typename INDEX = uint64_t>
  void naive_compute_minimal_substrings(string &text, std::unordered_map<INDEX, INDEX> &previousPositionSet, INDEX strlen, std::vector<std::string> &output)
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
          output.push_back(std::string(1, c));
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

            output.push_back(minimalSubstr);
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

    template <typename INDEX = uint64_t>
  std::vector<std::string> naive_compute_minimal_substrings(vector<char> &T)
  {
    string text(T.begin(), T.end());
    std::unordered_map<INDEX, INDEX> positionCountSet;
    std::vector<std::string> output;
    stool::naive_compute_minimal_substrings<INDEX>(text, positionCountSet, 1, output);
    for (INDEX len = 2; len <= text.size(); len++)
    {
      std::cout << "[" << len << "/" << positionCountSet.size() << "]\r" << std::flush;
      if (positionCountSet.size() == 0)
      {
        break;
      }
      else
      {
        stool::naive_compute_minimal_substrings<INDEX>(text, positionCountSet, len, output);
      }
    }
    std::cout << std::endl;
    std::sort(output.begin(), output.end());
    return output;
  }
} // namespace stool