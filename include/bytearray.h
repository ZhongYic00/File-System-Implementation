#ifndef BYTEARRAY_H
#define BYTEARRAY_H
#include "constants.h"
#include <bits/stdc++.h>
class ByteArray {
public:
    ByteArray();
    ByteArray(size_t size, Byte* data);
    ByteArray(const ByteArray& x);
    ByteArray(ByteArray&& x);
    ByteArray(const std::list<ByteArray>& x);
    template <typename T>
    ByteArray(const std::list<T>& l);
    ~ByteArray();
    inline const BytePtr d_ptr();
    inline const BytePtr d_ptr() const;
    inline size_t length() const;
    inline const BytePtr begin();
    inline const BytePtr end();
    inline const BytePtr operator[](const size_t& pos);
    inline const BytePtr operator[](const size_t& pos) const;

private:
    Byte* _d;
};
size_t ByteArray::length() const
{
    return _d ? *(size_t*)(_d) : 0;
}
const BytePtr ByteArray::d_ptr() { return _d ? _d + sizeof(size_t) : nullptr; } //bugs may occur
const BytePtr ByteArray::d_ptr() const { return _d ? _d + sizeof(size_t) : nullptr; } //bugs may occur
const BytePtr ByteArray::begin() { return _d ? _d + sizeof(size_t) : nullptr; }
const BytePtr ByteArray::end() { return _d ? _d + sizeof(size_t) + length() : nullptr; } //end() refers to the (length()+1)th pos of data area
const BytePtr ByteArray::operator[](const size_t& pos) { return _d ? _d + sizeof(size_t) + pos : nullptr; }
const BytePtr ByteArray::operator[](const size_t& pos) const { return _d ? _d + sizeof(size_t) + pos : nullptr; }
#endif
