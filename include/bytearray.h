#ifndef BYTEARRAY_H
#define BYTEARRAY_H
#include "constants.h"
#include <bits/stdc++.h>
class ByteArray {
public:
    ByteArray(size_t size, Byte* data);
    ByteArray(const ByteArray& x);
    ByteArray(ByteArray&& x);
    ByteArray(const std::list<ByteArray>& x);
    ~ByteArray();
    inline const BytePtr d_ptr() const;
    inline size_t length() const;
    inline const BytePtr begin();
    inline const BytePtr end();
    inline const BytePtr operator[](const size_t& pos) const;

private:
    Byte* _d;
};
size_t ByteArray::length() const
{
    return *(size_t*)(_d);
}
const BytePtr ByteArray::d_ptr() const
{
    return _d + sizeof(size_t);
}
const BytePtr ByteArray::begin() { return _d + sizeof(size_t); }
const BytePtr ByteArray::end() { return _d + sizeof(size_t) + length(); } //end() refers to the (length()+1)th pos of data area
const BytePtr ByteArray::operator[](const size_t& pos) const { return _d + sizeof(size_t) + pos; }
#endif
