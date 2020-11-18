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
    inline size_t totalSize();
    inline size_t metaSize();
    ull directSubnodes();
    ull subnodes();

private:
    Access access;
    ExtendedMap extAttributes;
    TimeManager tm;
    size_t _metaSize;

protected:
    size_t _totalSize;
    ull _subnodes;

    void updateMetaSize();
    virtual void updateTotalSize() = 0;
    virtual void updateSubnodes() = 0;
};

size_t FSNode::metaSize() { return _metaSize; }
size_t FSNode::totalSize() { return _totalSize; }

#endif // FSNODE_H
