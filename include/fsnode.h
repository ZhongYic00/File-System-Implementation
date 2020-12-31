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
        NodeInvalid = static_cast<inum_t>(-1),
        NodeNull = static_cast<inum_t>(-2)
    };
    FSNode();
    FSNode(const inum_t& type);
    FSNode(const ByteArray& d);
    virtual ~FSNode();
    inline size_t totalSize() const;
    inline size_t metaSize() const;
    ByteArray nodeDataExport() const;
    inline inum_t inum() const;
    inline LBA_t dataExtentLBA() const;
    inline bool isValid() const;
    inline bool isDirectory() const;
    //ull directSubnodes() const;
    inline ull refCount() const;
    inline void updateDataExtentLBA(const LBA_t& addr);
    inline void print() const;
    inline void chmod(mode_t m);
    inline mode_t mod() const;

protected:
    Access access;
    //ExtendedMap extAttributes;
    TimeManager tm;

private:
    const inum_t _inum;
    static inum_t nodeCount;
    static stack<inum_t> freeInum;

    static ByteArray freeInumExport();
    static void loadFreeInumPool(const ByteArray& d);

protected:
    LBA_t _dataLBA;
    size_t _totalSize;
    ull _refs; //refer to subnode count in dir, hard link count in file

    //void updateMetaSize();
    static void freeNodeNum(const inum_t& inum);

private:
    static inum_t newNodeNum();
};

//size_t FSNode::metaSize() const { return _metaSize; }
size_t FSNode::totalSize() const { return _totalSize; }
bool FSNode::isValid() const { return access.isValid(); }
bool FSNode::isDirectory() const { return access.isDirectory(); }
LBA_t FSNode::dataExtentLBA() const { return _dataLBA; }
ull FSNode::refCount() const { return _refs; }
inum_t FSNode::inum() const { return _inum; }
void FSNode::updateDataExtentLBA(const LBA_t& addr) { _dataLBA = addr; }
void FSNode::chmod(mode_t m) { access.chmod(m); }
mode_t FSNode::mod() const { return access.mod(); }
void FSNode::print() const { cerr << "inum:" << inum() << " ref:" << refCount() << " dLBA:" << _dataLBA << endl; }
#endif // FSNODE_H
