#ifndef EXTENTTREE_H
#define EXTENTTREE_H
#include "constants.h"
#include "fsnode.h"
using std::list;
class ExtentTree {
public:
    ExtentTree(const ByteArray& d);
    ByteArray dataExport() const;
    list<pair<LBA_t, LBA_t>> allocateExtents(LBA_t blks); //allocate extents by number of blocks, return extents sorted by ascending order(first:LBA,second:extentsize)
};
#endif
