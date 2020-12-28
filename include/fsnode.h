#ifndef FSNODE_H
#define FSNODE_H
#include "access.h"
#include "bytearray.h"
#include "constants.h"
#include "extendedmap.h"
#include "timemanager.h"
#include <bits/stdc++.h>
typedef unsigned long long ull;
typedef ull inum_t;
using std::stack;
using std::string;
class FS;
//typedef string ByteArray;
class FSNode {
    friend class FS;

public:
    enum NodeTypes {
        NodeInvalid
    };
    FSNode();
    FSNode(const inum_t& type);
    FSNode(const ByteArray& d);
    ~FSNode();
    inline size_t totalSize() const;
    inline size_t metaSize() const;
    inum_t inum() const;
    ull directSubnodes() const;
    ull subnodes() const;
    ByteArray nodeDataExport() const;
    inline LBA_t dataExtentLBA() const;
    inline bool invalid() const;
    inline bool isDirectory() const;

private:
    Access access;
    ExtendedMap extAttributes;
    TimeManager tm;
    const inum_t _inum;
    static inum_t nodeCount;
    static stack<inum_t> freeInum;

protected:
    LBA_t _dataLBA;
    size_t _totalSize;
    ull _subnodes; //refer to subnode count in dir, hard link count in file

    void updateMetaSize();
    static void freeNodeNum(const inum_t& inum);

private:
    static inum_t newNodeNum();
};

//size_t FSNode::metaSize() const { return _metaSize; }
size_t FSNode::totalSize() const { return _totalSize; }
bool FSNode::isValid() const { return access.isValid(); }
bool FSNode::isDirectory() const { return access.isDirectory(); }
LBA_t FSNode::dataExtentLBA() const { return _dataLBA; }
#endif // FSNODE_H
