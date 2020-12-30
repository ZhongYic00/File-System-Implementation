#include "include/bytearray.h"
ByteArray::ByteArray()
    : _d(nullptr)
{
}
ByteArray::ByteArray(size_t size, Byte* data)
    : _d(new Byte[size + sizeof(size_t)])
{
    *(size_t*)(_d) = size;
    memcpy(_d + sizeof(size_t), data, size);
}
ByteArray::ByteArray(const ByteArray& x)
    : _d(new Byte[x.length() + sizeof(size_t)])
{
    memcpy(_d, x._d, x.length() + sizeof(size_t));
}
ByteArray::ByteArray(ByteArray&& x)
    : _d(x._d)
{
    x._d = nullptr;
}
ByteArray::ByteArray(const std::list<ByteArray>& x)
{
    size_t l = 0;
    for (auto i : x)
        l += i.length();
    _d = new Byte[sizeof(size_t) + l];
    *reinterpret_cast<size_t*>(_d) = l;
    l = 0;
    for (auto i : x) {
        memcpy(_d + l, i.d_ptr(), i.length());
        l += i.length();
    }
}
template <typename T>
ByteArray::ByteArray(const std::list<T>& l)
    : _d(new Byte[l.size() * sizeof(T)] + sizeof(size_t))
{
    *reinterpret_cast<size_t*>(_d) = l.size() * sizeof(T);
    auto d = _d + sizeof(size_t);
    for (auto i : l) {
        *reinterpret_cast<T*>(d) = i;
        d++;
    }
}
ByteArray::~ByteArray()
{
    delete[] _d;
}
