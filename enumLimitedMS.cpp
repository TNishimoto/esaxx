// License: MIT http://opensource.org/licenses/MIT
/*
  This code was copied from https://takeda25.hatenablog.jp/entry/20101202/1291269994 and I modified it.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "cmdline.h"
#include "esa.hxx"

using namespace std;
using INDEXTYPE = int64_t;

using LCPPair = std::pair<stool::LCPInterval<INDEXTYPE>,std::pair<uint64_t,uint64_t>>;


template <typename sa_type>
string toString(LCPPair& item, vector<char> &text, sa_type &sa){
        string result = "";
        uint64_t sum_y = item.second.first + item.second.second;
        uint64_t plus_count_y = item.second.first;
        double y_value = (double)plus_count_y / (double)sum_y;
        uint64_t plus_value = item.second.first;
        uint64_t minus_value = item.second.second;
        string line = item.first.getText(text, sa);

        //result += std::to_string(y_value) + "," + std::to_string(plus_value) + "," + std::to_string(minus_value) + "," + std::to_string(sum_y) + "," + std::to_string(item.first.lcp) + "," + translate( line);
        result += std::to_string(y_value) + "," + line;
        
        return result;
}
vector<char> getInputText(string filepath)
{
    std::ifstream ifs(filepath);
    bool inputFileExist = ifs.is_open();
    if (!inputFileExist)
    {
        std::cout << filepath << " cannot open." << std::endl;
        throw - 1;
    }

    std::string tmp;
    vector<char> result;
    std::cout << "Loading the input text..." << std::endl;

    while (getline(ifs, tmp))
    {
        for (char c : tmp)
        {
            result.push_back(c);
        }
        result.push_back((char)1);
    }
    result.pop_back();
    result.push_back((char)0);
    return result;
}
vector<INDEXTYPE> constructExcludedPositionRanks(vector<char> &text)
{
    vector<INDEXTYPE> tmp;
    //vector<INDEXTYPE> result;
    tmp.resize(text.size(), UINT64_MAX);

    uint64_t prev=0;
    uint64_t rank=0;
    for (uint64_t i = 0; i < text.size(); i++)
    {
        if (text[i] == (char)1 || text[i] == '+' || text[i] == '-')
        {
            for(uint64_t j=prev;j<i;j++){
                tmp[j] = rank;
            }
            rank++;
            prev = i;            
        }
    }
    return tmp;
}
INDEXTYPE getBorderPosition(vector<char> &text)
{
    vector<INDEXTYPE> result;
    for (uint64_t i = 0; i < text.size(); i++)
    {
        if (text[i] == '-')
        {
            return i;
        }
    }
    return text.size();
}
template <typename sa_type>
bool isOK(stool::LCPInterval<INDEXTYPE> &interval, sa_type &sa, vector<INDEXTYPE> &excludedPositions)
{
    INDEXTYPE start_pos = sa[interval.i];
    INDEXTYPE end_pos = sa[interval.i] + interval.lcp - 1;

    bool b1, b2;

    if(start_pos == 0){
        if(excludedPositions[0] == 0){
            b1 = true;
        }else{
            b1 = false;
        }
    }else{
        b1 = (excludedPositions[start_pos] - excludedPositions[start_pos-1]) == 0;
    }
        b2 = (excludedPositions[end_pos] - excludedPositions[start_pos]) == 0;


    return b1 && b2;
}
template <typename sa_type>
std::pair<uint64_t,uint64_t> getFrequency(stool::LCPInterval<INDEXTYPE> &interval, sa_type &sa, uint64_t border){
    //vector<uint64_t> occurrences;
    uint64_t plus = 0;
    uint64_t minus = 0;
    for(uint64_t i=interval.i;i<=interval.j;i++){
        if(sa[i] < border){
            plus++;
        }else{
            minus++;
        }
        //occurrences.push_back(sa[i]);
    }
    return std::pair<uint64_t,uint64_t>(plus, minus);
}

int main(int argc, char *argv[])
{

    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("output_file", 'o', "output file name", false, "");
    p.add<bool>("print", 'p', "print info", false, true);

    p.add<int64_t>("occ_threshold", 'a', "the threshold of occurrences", false, 0);
    p.add<int64_t>("len_threshold", 'b', "the threshold of length", false, 0);
    p.add<int64_t>("occlen_threshold", 'c', "the threshold of occ length", false, 0);


    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string outputFile = p.get<string>("output_file");
    //string format = p.get<string>("format");
    int64_t occ_threshold = p.get<int64_t>("occ_threshold");
    int64_t len_threshold = p.get<int64_t>("len_threshold");
    int64_t occlen_threshold = p.get<int64_t>("occlen_threshold");

    bool isPrint = p.get<bool>("print");

    if (outputFile.size() == 0)
    {
        outputFile = inputFile + "."+ std::to_string(occ_threshold) + "." + std::to_string(len_threshold) + "." + std::to_string(occlen_threshold) + ".csv";
    }

    vector<char> T = getInputText(inputFile); // input text
    vector<INDEXTYPE> excludedPositions = constructExcludedPositionRanks(T);
    uint64_t border = getBorderPosition(T);
    INDEXTYPE n = T.size();

    vector<INDEXTYPE> SA(n); // suffix array
    vector<INDEXTYPE> L(n);  // left boundaries of internal node
    vector<INDEXTYPE> R(n);  // right boundaries of internal node
    vector<INDEXTYPE> D(n);  // depths of internal node

    INDEXTYPE alphaSize = 0x100; // This can be very large
    INDEXTYPE nodeNum = 0;

    // Computing internal nodes of the suffix tree of the input file.
    if (esaxx(T.begin(), SA.begin(),
              L.begin(), R.begin(), D.begin(),
              n, alphaSize, nodeNum) == -1)
    {
        return -1;
    }

    INDEXTYPE size = T.size();
    INDEXTYPE SA_first_index = 0;

    vector<INDEXTYPE> rank(size);
    INDEXTYPE r = 0;
    for (INDEXTYPE i = 0; i < size; i++)
    {
        if (SA[i] == 0)
            SA_first_index = i;
        if (i == 0 || T[(SA[i] + size - 1) % size] != T[(SA[i - 1] + size - 1) % size])
        {
            r++;
        }
        rank[i] = r;
    }

    if (isPrint)
    {
        std::cout << "Maximal substrings in the file" << std::endl;
        std::cout << "id"
                  << "\t\t"
                  << "occurrence"
                  << "\t"
                  << "range(SA)"
                  << "\t"
                  << "string length"
                  << "\t"
                  << "string" << std::endl;
    }

    vector<LCPPair> buffer;

    ofstream os(outputFile, ios::out | ios::binary);
    if (!os)
        return 1;
    INDEXTYPE maximumSubstringCount = 0;
    INDEXTYPE id = 0;

    // Filtering internal nodes and writing and printing maximal substrings.
    INDEXTYPE line_id = 0;

    for (INDEXTYPE i = 0; i < nodeNum; ++i)
    {
        stool::LCPInterval<INDEXTYPE> interval(L[i], R[i], D[i]);
        INDEXTYPE len = D[i];
        if ((rank[interval.j - 1] - rank[interval.i] == 0 || !isOK(interval, SA, excludedPositions)))
        {
            continue;
        }
        std::pair<uint64_t,uint64_t> freq = getFrequency(interval, SA, border);
        if ( freq.first + freq.second >= occ_threshold && interval.lcp >= len_threshold && (interval.lcp*(freq.first + freq.second) >= occlen_threshold ))
        {
        }else{
            continue;
        }
        
        id++;
        maximumSubstringCount++;


        buffer.push_back(LCPPair(interval, freq) );

    }

    std::sort(buffer.begin(), buffer.end(),
              [](const LCPPair &x, const LCPPair &y) {
                  uint64_t sum_x = x.second.first + x.second.second;
                  uint64_t plus_count_x = x.second.first;
                  double x_value = (double)plus_count_x / (double)sum_x;

                  uint64_t sum_y = y.second.first + y.second.second;
                  uint64_t plus_count_y = y.second.first;
                  double y_value = (double)plus_count_y / (double)sum_y ;
                  if(plus_count_x == 0 && plus_count_y == 0){
                      return sum_x < sum_y; 
                  }else{
                      return x_value > y_value;
                  }
              });

    os << "Positive example ratio, maximal substring" << std::endl;
    //os << "Positive example ratio, the number of positive examples, the number of negative example, the number of occurrences, the length of maximal substring, maximal substring" << std::endl;
    for (uint64_t z = 0; z < buffer.size(); z++)
    {
        LCPPair &item = buffer[z];

        uint64_t sum_y = item.second.first + item.second.second;
        uint64_t plus_count_y = item.second.first;
        double y_value = (double)plus_count_y / (double)sum_y;
        uint64_t plus_value = item.second.first;
        uint64_t minus_value = item.second.second;

        os << toString(item, T, SA);
        if (z + 1 < buffer.size())
            os << std::endl;
    }
    buffer.clear();
    os.close();

    std::cout << "\033[36m";
    std::cout << "___________RESULT___________" << std::endl;
    std::cout << "File: " << inputFile << std::endl;
    std::cout << "Output: " << outputFile << std::endl;
    std::cout << "Occurrence threshold: " << occ_threshold << std::endl;
    std::cout << "Length threshold: " << len_threshold << std::endl;
    std::cout << "Occurrence*Length threshold: " << occlen_threshold << std::endl;

    std::cout << "The length of the input text: " << T.size() << std::endl;
    std::cout << "The number of maximum substrings: " << maximumSubstringCount << std::endl;
    std::cout << "_________________________________" << std::endl;
    std::cout << "\033[39m" << std::endl;
}