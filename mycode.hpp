#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "divsufsort.h"
#include "divsufsort64.h"

namespace stool{
class LCPInterval
{
  public:
    uint64_t i;
    uint64_t j;
    uint64_t lcp;
    LCPInterval(){}
    LCPInterval(uint64_t _i, uint64_t _j, uint64_t _lcp)
    {
    this->i = _i;
    this->j = _j;
    this->lcp = _lcp;
    }
};

/**
 * @brief Constructs the suffix array of a given string in linear time.
 * @param T[0..n-1] The input string. (random access iterator)
 * @param SA[0..n-1] The output array of suffixes. (random access iterator)
 * @param n The length of the given string.
 * @param k The alphabet size.
 * @return 0 if no error occurred, -1 or -2 otherwise.
 */
template<typename string_type, typename sarray_type, typename index_type>
int saisxx(string_type T, sarray_type SA, index_type n){

}
template <>
int saisxx<std::vector<char>::iterator, std::vector<int32_t>::iterator,int32_t>(std::vector<char>::iterator T, std::vector<int32_t>::iterator SA, int32_t n){
  //if((n < 0) || (k <= 0)) { return -1; }
  if(n <= 1) { if(n == 1) { SA[0] = 0; } return 0; }

  int err = divsufsort((const unsigned char *)&T[0], (int32_t *)&SA[0], n);
  return err;
}
template <>
int saisxx<std::vector<char>::iterator, std::vector<int64_t>::iterator,int64_t>(std::vector<char>::iterator T, std::vector<int64_t>::iterator SA, int64_t n){
  //if((n < 0) || (k <= 0)) { return -1; }
  if(n <= 1) { if(n == 1) { SA[0] = 0; } return 0; }

  int err = divsufsort64((const unsigned char *)&T[0], (int64_t *)&SA[0], n);
  return err;
}

/*
template <typename ITEM, typename ITERATOR>
static uint64_t online_write(ofstream &os, ITEM &item)
{
		uint64_t count = 0;
		vector<ITEM> buffer;
		uint64_t print_counter = 0;
		std::cout << "Writing " << msg << std::flush;
		while (beginIt != endIt)
		{
			ITEM item = *beginIt;
			buffer.push_back(item);
			count++;
			if(print_counter != 0){
				--print_counter;
			}else{
				std::cout << "." << std::flush;
				print_counter = 100000;
			}

			if (buffer.size() > bufferSize)
			{
				os.write((const char *)(&buffer[0]), sizeof(ITEM) * buffer.size());
				buffer.clear();
			}

			++beginIt;
		}		
		os.write((const char *)(&buffer[0]), sizeof(ITEM) * buffer.size());
		buffer.clear();
		std::cout << "[END]" << std::endl;
		return count;
}
*/


}