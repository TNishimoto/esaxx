#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>
//#include "common/io.h"
//#include "common/print.hpp"
//#include "other_functions.hpp"
//#include "OnlineRlbwt/online_rlbwt.hpp"
//#include "rlbwt.hpp"
//#include "stool/src/elias_fano_vector.hpp"
#include "stool/src/io.h"

namespace stool
{
    namespace rlbwt2
    {
        static uint64_t count_runs(std::string filename)
        {
            std::ifstream inp;
            std::vector<char> buffer;
            uint64_t bufferSize = 8192;
            buffer.resize(8192);

            inp.open(filename, std::ios::binary);
            bool inputFileExist = inp.is_open();
            if (!inputFileExist)
            {
                std::cout << filename << " cannot open." << std::endl;

                throw std::runtime_error("error");
            }
            uint64_t textSize = stool::FileReader::getTextSize(inp);
            uint8_t prevChar = 255;
            uint64_t x = 0;
            uint64_t count_run = 0;
            while (true)
            {
                bool b = stool::FileReader::read(inp, buffer, bufferSize, textSize);
                if (!b)
                {
                    break;
                }

                for (uint64_t i = 0; i < buffer.size(); i++)
                {
                    uint8_t c = buffer[i];
                    if (prevChar != c || x == 0)
                    {
                        count_run++;
                        prevChar = c;
                    }
                    x++;
                }
            }
            inp.close();
            return count_run;
        }

        static void load_RLBWT_from_file(std::string filename, sdsl::int_vector<> &diff_char_vec, std::vector<bool> &run_bits)
        {

            std::ifstream inp;
            std::vector<char> buffer;
            uint64_t bufferSize = 8192;
            buffer.resize(8192);

            uint64_t runCount = count_runs(filename);
            diff_char_vec.resize(runCount);

            inp.open(filename, std::ios::binary);
            bool inputFileExist = inp.is_open();
            if (!inputFileExist)
            {
                std::cout << filename << " cannot open." << std::endl;

                throw std::runtime_error("error");
            }
            uint64_t textSize = stool::FileReader::getTextSize(inp);
            uint8_t prevChar = 255;
            uint64_t x = 0;
            uint64_t currentRunP = 0;
            while (true)
            {
                bool b = stool::FileReader::read(inp, buffer, bufferSize, textSize);
                if (!b)
                {
                    break;
                }

                for (uint64_t i = 0; i < buffer.size(); i++)
                {
                    assert(buffer[i] >=0);
                    uint8_t c = buffer[i];
                    if (prevChar != c || x == 0)
                    {
                        run_bits.push_back(1);
                        diff_char_vec[currentRunP++] = c;
                        prevChar = c;
                    }
                    else
                    {
                        run_bits.push_back(0);
                    }
                    x++;
                }
            }

            run_bits.push_back(1);

            inp.close();
        }
        static std::vector<uint64_t> construct_lpos_array(std::vector<bool> &run_bits)
        {
            std::vector<uint64_t> r;
            for (uint64_t i = 0; i < run_bits.size(); i++)
            {
                if (run_bits[i])
                {
                    r.push_back(i);
                }
            }
            return r;
        }
    } // namespace rlbwt2
} // namespace stool