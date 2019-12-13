#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <exception>


namespace stool
{
namespace esaxx
{

template <typename CHAR = char, typename TEXT = std::vector<CHAR>, typename SA = std::vector<uint64_t>>
class ForwardBWT
{
    public:
    class iterator
    {
    private:
        TEXT *_text;
        SA *_sa;
        uint64_t _index = 0;

    public:
        iterator() = default;
        iterator(TEXT *__text, SA *__sa, bool _isBegin) : _text(__text), _sa(__sa)
        {
            if (!_isBegin)
            {
                this->_index = __text->size();
            }
        }

    public:
        iterator &operator++()
        {
            ++this->_index;
            return *this;
        }
        CHAR operator*()
        {
            uint64_t p = (*this->_sa)[this->_index];
            if( p != 0 ){
                return (*_text)[p-1];
            }else{
                return (*_text)[_text->size()-1];
            }
        }
        bool operator!=(const iterator &rhs) const
        {
            return _index != rhs._index;
        }
    };

private:
    TEXT *_text;
    SA *_sa;

public:
    using const_iterator = iterator;
    ForwardBWT(TEXT *__text, SA *__sa) : _text(__text), _sa(__sa)
    {
    }
    auto begin() const -> iterator
    {

        auto it = iterator(this->_text, this->_sa, true);
        return it;
    }

    auto end() const -> iterator
    {
        auto it = iterator(this->_text, this->_sa, false);
        return it;
    }
};
} // namespace esaxx
} // namespace stool