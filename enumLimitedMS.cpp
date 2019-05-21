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

double kai(double a, double b, double c, double d){
    double sum_ab = a + b;
    double sum_cd = c + d;
    double sum_ac = a + c;
    double sum_bd = b + d;
    double sum_abcd = a + b + c + d;

    double expected_a = sum_ab * (sum_ac / sum_abcd);
    double expected_b = sum_ab * (sum_bd / sum_abcd);
    double expected_c = sum_cd * (sum_ac / sum_abcd);
    double expected_d = sum_cd * (sum_bd / sum_abcd);

    double kai_a = ((a - expected_a) * (a - expected_a)) / expected_a;
    double kai_b = ((b - expected_b) * (b - expected_b)) / expected_b;
    double kai_c = ((c - expected_c) * (c - expected_c)) / expected_c;
    double kai_d = ((d - expected_d) * (d - expected_d)) / expected_d;
    return kai_a + kai_b + kai_c + kai_d;
}
bool kaip(double a, double b, double c, double d){
    double sum_ab = a + b;
    double sum_cd = c + d;
    double sum_ac = a + c;
    double sum_bd = b + d;
    double sum_abcd = a + b + c + d;

    double expected_a = sum_ab * (sum_ac / sum_abcd);
    double expected_b = sum_ab * (sum_bd / sum_abcd);
    bool b_a = a >= expected_a;
    return b_a;
}

template <typename sa_type>
string toString(LCPPair& item, vector<char> &text, sa_type &sa, std::pair<uint64_t,uint64_t> &sum_info){
        string result = "";
        uint64_t sum_y = item.second.first + item.second.second;
        uint64_t plus_count_y = item.second.first;
        double y_value = (double)plus_count_y / (double)sum_y;
        uint64_t plus_value = item.second.first;
        uint64_t minus_value = item.second.second;
        string line = item.first.getText(text, sa);

        uint64_t other_true_sum = sum_info.first - plus_value;
        uint64_t other_false_sum = sum_info.second - minus_value;

        double kai_value = kai(plus_value, minus_value, other_true_sum, other_false_sum);
        bool bb = kaip(plus_value, minus_value, other_true_sum, other_false_sum);
        string b1 = (bb ? "+" : "-");

        result += std::to_string(kai_value) + "," + b1 + ","+ std::to_string(plus_value) + "," + std::to_string(minus_value) + "," + std::to_string(other_true_sum) + "," + std::to_string(other_false_sum) + "," + line;
        
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
    /*
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
    */

    vector<LCPPair> buffer;

    ofstream os(outputFile, ios::out | ios::binary);
    if (!os)
        return 1;
    INDEXTYPE maximumSubstringCount = 0;
    INDEXTYPE id = 0;

    // Filtering internal nodes and writing and printing maximal substrings.
    INDEXTYPE line_id = 0;

    uint64_t true_sum = 0;
    uint64_t false_sum = 0;


    for (INDEXTYPE i = 0; i < nodeNum; ++i)
    {
        stool::LCPInterval<INDEXTYPE> interval(L[i], R[i], D[i]);
        INDEXTYPE len = D[i];
        if ((rank[interval.j - 1] - rank[interval.i] == 0 || !isOK(interval, SA, excludedPositions)) || interval.lcp == 0 )
        {
            continue;
        }
        std::pair<uint64_t,uint64_t> freq = getFrequency(interval, SA, border);
        true_sum += freq.first;
        false_sum += freq.second;

        if ( freq.first + freq.second >= occ_threshold && interval.lcp >= len_threshold && (interval.lcp*(freq.first + freq.second) >= occlen_threshold) )
        {
        }else{
            continue;
        }
        
        id++;
        maximumSubstringCount++;


        buffer.push_back(LCPPair(interval, freq) );

    }

    std::pair<uint64_t, uint64_t> zikoPair = std::pair<uint64_t, uint64_t>(true_sum, false_sum);


    std::sort(buffer.begin(), buffer.end(),
              [&](const LCPPair &x, const LCPPair &y) {
                  double kai_x = kai(x.second.first, x.second.second, true_sum - x.second.first, false_sum - x.second.second);
                  double kai_y = kai(y.second.first, y.second.second, true_sum - y.second.first, false_sum - y.second.second);
                  return kai_y < kai_x;
              });
    string column0 = "Kai";
    string column1 = "Bias";
    string column2 = "MS(TRUE)";
    string column3 = "MS(FALSE)";
    string column4 = "Others(TRUE)";
    string column5 = "Others(FALSE)";
    string column6 = "Maximal_substring";

    os << column0 << "," << column1 << ", " << column2 << ", " << column3 << ", " << column4 << ", " << column5 << "," << column6 << std::endl;
    for (uint64_t z = 0; z < buffer.size(); z++)
    {
        LCPPair &item = buffer[z];
        os << toString(item, T, SA, zikoPair);
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