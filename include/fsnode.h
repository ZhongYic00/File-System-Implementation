#ifndef FSNODE_H
#define FSNODE_H
#include "access.h"
#include "extendedmap.h"
#include "timemanager.h"
#include <bits/stdc++.h>
typedef unsigned long long ull;
class FSNode {
public:
    FSNode();
    ~FSNode();
    size_t bodySize();
    size_t totalSize();
    ull directSubnodes();
    ull subNodes();

private:
    Access access;
    ExtendedMap extAttributes;
    TimeManager tm;
    size_t metaSize;
    size_t _totalSize;
    ull subnodes;

    void updateMetaSize();
    virtual void updateBodySize() = 0;
    virtual void updateTotalSize() = 0;
    virtual void updateSubnodes() = 0;
};

#endif // FSNODE_H
