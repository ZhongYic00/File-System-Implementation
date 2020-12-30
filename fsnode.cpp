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
FSNode::FSNode(const ByteArray& d)
    : _inum(*reinterpret_cast<inum_t*>(d[18]))
{
    memcpy(&access, d.d_ptr(), sizeof(*this));
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
ByteArray FSNode::freeInumExport()
{
    list<inum_t> l;
    for (; !freeInum.empty(); freeInum.pop())
        l.push_back(freeInum.top());
    auto tmp = new Byte[l.size() * sizeof(inum_t) + sizeof(size_t)];
    *reinterpret_cast<size_t*>(tmp) = l.size();
    auto j = reinterpret_cast<inum_t*>(tmp + sizeof(size_t));
    for (auto i : l) {
        *j = i;
        j++;
    }
    auto rt = ByteArray(l.size() * sizeof(inum_t) + sizeof(size_t), tmp);
    return rt;
}
void FSNode::loadFreeInumPool(const ByteArray& d)
{
    auto n = *reinterpret_cast<size_t*>(d[0]);
    for (int i = 0; i < n; i++) {
        freeInum.push(*reinterpret_cast<inum_t*>(d[i * sizeof(inum_t) + sizeof(size_t)]));
    }
}
