#include "include/fsnode.h"

stack<inum_t> FSNode::freeInum;
inum_t FSNode::nodeCount = 1;

FSNode::FSNode()
    : _inum(newNodeNum())
    , _dataLBA(0)
    , _totalSize(0)
    , _refs(1)
{
}
FSNode::FSNode(const inum_t& type)
    : _inum(type)
    , _dataLBA(0)
    , _totalSize(0)
    , _refs(0)
{
}
FSNode::~FSNode() {}
inum_t FSNode::newNodeNum()
{
    if (freeInum.empty())
        return nodeCount++; //may exists bug when it comes to interruption
    inum_t rt = freeInum.top();
    return freeInum.pop(), rt;
}
void FSNode::freeNodeNum(const inum_t& inum)
{
    freeInum.push(inum);
}
ByteArray FSNode::nodeDataExport() const
{
    BytePtr tmp = new Byte[sizeof(*this)];
    memcpy(tmp, this, sizeof(*this));
    ByteArray rt(sizeof(*this), tmp);
    delete[] tmp;
    return rt;
}
