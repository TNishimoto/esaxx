#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <vector>
#include <type_traits>
//#include "./rinterval_storage.hpp"
#include "./range_distinct_on_rlbwt.hpp"

namespace stool
{
    namespace lcp_on_rlbwt
    {

        template <typename INDEX_SIZE, typename LPOSDS, typename FPOSDS>
        class RLBWTDataStructures
        {
        public:
            /*
            using CHARVEC = typename RLBWT_STR::char_vec_type;
            */

            using INDEX = INDEX_SIZE;
            using RINTERVAL = RInterval<INDEX_SIZE>;
            using CHAR = uint8_t;
            using UCHAR = typename std::make_unsigned<CHAR>::type;
            const sdsl::int_vector<> &bwt;
            sdsl::wt_huff<> &wt;
            const LPOSDS &lpos_vec;
            const FPOSDS &_fposDS;
            RangeDistinctDataStructureOnRLBWT<INDEX_SIZE, LPOSDS > rangeOnRLBWT;

            //const RLBWT_STR &_rlbwt;

            RLBWTDataStructures(const sdsl::int_vector<> &diff_char_vec,
                                sdsl::wt_huff<> &_wt, const LPOSDS &_lpos_vec, const FPOSDS &__fposDS) : bwt(diff_char_vec), wt(_wt), lpos_vec(_lpos_vec), _fposDS(__fposDS)
            {

                rangeOnRLBWT.initialize(&wt, &bwt, &lpos_vec);                
            }

            INDEX_SIZE get_fpos(INDEX_SIZE index, INDEX_SIZE diff) const
            {
                return _fposDS[index] + diff;
            }
            uint64_t get_lindex_containing_the_position(uint64_t lposition) const
            {
                auto p = std::upper_bound(this->lpos_vec.begin(), this->lpos_vec.end(), lposition);
                INDEX pos = std::distance(this->lpos_vec.begin(), p) - 1;
                return pos;
            }
            uint64_t str_size() const
            {
                return lpos_vec[lpos_vec.size() - 1];
            }
            uint8_t get_char_by_run_index(uint64_t _run_index) const
            {
                return bwt[_run_index];
            }
            uint64_t rle_size() const
            {
                return bwt.size();
            }
            uint64_t get_run(uint64_t i) const
            {
                return lpos_vec[(i + 1)] - lpos_vec[i];
            }
            uint64_t get_lpos(uint64_t i) const
            {
                return lpos_vec[i];
            }

            uint64_t get_end_rle_lposition() const
            {
                for (INDEX i = 0; i < bwt.size(); i++)
                {
                    if (bwt[i] == 0)
                    {
                        return i;
                    }
                }
                return std::numeric_limits<INDEX>::max();
            }

            RINTERVAL getIntervalOnL(const RINTERVAL &interval) const
            {
                RINTERVAL output;
                INDEX_SIZE begin_pos = this->get_fpos(interval.beginIndex, interval.beginDiff);
                output.beginIndex = this->get_lindex_containing_the_position(begin_pos);
                output.beginDiff = begin_pos - lpos_vec[output.beginIndex];

                INDEX_SIZE end_pos = this->get_fpos(interval.endIndex, interval.endDiff);

                output.endIndex = this->get_lindex_containing_the_position(end_pos);
                output.endDiff = end_pos - lpos_vec[output.endIndex];
                return output;
            }
        };
    } // namespace lcp_on_rlbwt
} // namespace stool