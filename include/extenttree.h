#ifndef EXTENTTREE_H
#define EXTENTTREE_H
#include "constants.h"
#include "fsnode.h"
#include <bits/stdc++.h>
const int SumBlo = 1 << 30;
struct node {
    int num, bl, logbl, size, left, maxleft, ref;
    LBA_t flag;
    node *l, *r;
    node()
    {
        flag = 0;
        num = bl = logbl = size = left = maxleft = ref = 0;
        l = r = NULL;
    }
};
class ExtentTree {
public:
    ExtentTree(const ByteArray& d);
    ByteArray dataExport() const;
    list<pair<LBA_t, LBA_t>> allocateExtents(LBA_t blks);
    list<pair<LBA_t, LBA_t>> allocateExtent(int references);
    void releaseExtent(LBA_t startpos, LBA_t length);
    inline void releaseExtent(list<pair<LBA_t, LBA_t>> extents);

private:
    static int cnt, numt;
    static node* root;
    //ByteArray dataExport();
    //allocate extents by number of blocks, return extents sorted by ascending order(first:LBA,second:extentsize)
};
#endif
