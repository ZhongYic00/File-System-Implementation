#include "include/inodemap.h"

using namespace std;

InodeMap::InodeMap(const ByteArray& d)
{
    if (!d.length())
        return;
    size_t sz = *reinterpret_cast<size_t*>(d[0]);
    for (int i = 0; i < sz; i++) {
        auto cur = reinterpret_cast<pair<inum_t, LBA_t>*>(d[i * sizeof(pair<inum_t, LBA_t>) + sizeof(size_t)]);
        inodes[cur->first] = cur->second;
    }
}
//Notes here

ByteArray InodeMap::dataExport()
{
    BytePtr tmp = new Byte[inodes.size() + sizeof(size_t)];
    *reinterpret_cast<size_t*>(tmp) = inodes.size();
    auto p = reinterpret_cast<pair<inum_t, LBA_t>*>(tmp + sizeof(size_t));
    for (auto i : inodes) {
        *p = i;
        p++;
    }
    auto rt = ByteArray(inodes.size() + sizeof(size_t), tmp);
    delete[] tmp;
    return rt;
}
//Notes here
