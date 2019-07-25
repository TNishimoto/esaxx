
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "cmdline.h"
#include "esa.hxx"
#include "postorder_suffix_tree.hpp"
#include "minimal_substrings/postorder_special_suffix_tree.hpp"
#include "minimal_substrings/minimal_substring_iterator.hpp"
#include "minimal_substrings/naive_minimal_substrings.hpp"
#include "minimal_substrings/minimal_substring_tree.hpp"

#include "sa.hpp"

using namespace std;
using INDEXTYPE = uint64_t;






int main(int argc, char *argv[])
{
  cmdline::parser p;
  p.add<string>("input_file", 'i', "input file name", true);
  p.add<string>("output_file", 'o', "(option) Output attractor file name(the default output name is 'input_file.msub')", false, "");
  p.add<string>("output_type", 't', "(option) Output mode(binary or text)", false, "binary");
  //p.add<bool>("print", 'p', "print info", false, true);

  p.parse_check(argc, argv);
  string inputFile = p.get<string>("input_file");
  string outputFile = p.get<string>("output_file");
  string outputMode = p.get<string>("output_type");
  if (outputFile.size() == 0)
  {
    if (outputMode == "text")
    {
      outputFile = inputFile + ".msub.txt";
    }
    else
    {
      outputFile = inputFile + ".msub";
    }
  }
  if(outputMode != "text") outputMode = "binary";

  vector<uint8_t> T = stool::load_text2(inputFile); // input text

  stool::esaxx::MinimalSubstringTree<uint8_t, uint64_t> mstree;
  stool::esaxx::MinimalSubstringTree<uint8_t, uint64_t>::construct(T, mstree.nodes, mstree.parents);
  
  if (outputMode == "text")
  {
    T.pop_back();
    vector<uint64_t> sa = stool::constructSA<>(T);
    string otext = "";
    
    for (uint64_t i = 0; i < mstree.nodes.size(); i++)
    {
      otext.append(stool::esaxx::toLogLine<>(T, sa, mstree.nodes[i]));
      if (i + 1 != mstree.nodes.size())
        otext.append("\r\n");
    }
    //IO::write(outputFile, otext);
		std::ofstream out(outputFile, ios::out | ios::binary);
		out.write((const char *)(&otext[0]), sizeof(char) * otext.size());
    
  }
  else
  {
    mstree.write(outputFile, T);
  }
  std::cout << "\033[36m";
  std::cout << "___________RESULT___________" << std::endl;
  std::cout << "File: " << inputFile << std::endl;
  std::cout << "Output: " << outputFile << std::endl;
  std::cout << "Output format: " << outputMode << std::endl;
  std::cout << "The length of the input text: " << T.size() << std::endl;
  //std::cout << "The number of maximum substrings: " << maximumSubstringCount << std::endl;
  std::cout << "The number of minimum substrings: " << mstree.nodes.size() << std::endl;
  std::cout << "_________________________________" << std::endl;
  std::cout << "\033[39m" << std::endl;
}