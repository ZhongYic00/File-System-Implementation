#include "include/fsnodes.h"
DirectoryNode::DirectoryNode() {}
DirectoryNode::DirectoryNode(const FSNode& node, const ByteArray& ext)
    : FSNode(node)
{
    access.setDirectory();
    if (!ext.length())
        return;
    size_t sz = *reinterpret_cast<size_t*>(ext[0]);
    for (int i = 0; i < sz; i++) {
        auto attr = reinterpret_cast<NodeCoreAttr*>(ext[sizeof(size_t)])[i];
        subnodeAttr[calcHash(attr.name)] = attr;
    }
    //recover subnodeAttr from ext
}
void DirectoryNode::addSubnode(const inum_t& addr, const string& nodeName)
{
    hash_t hsh = calcHash(nodeName);
    if (subnodeAttr.count(hsh)) {
        if (subnodeAttr[hsh].name == nodeName) {
            throw "subnode already exists";
        } else {
            throw "hash value conflicts with \"" + subnodeAttr[hsh].name + '\"';
        }
    } else {
        subnodeAttr[hsh] = { addr, nodeName };
    }
    tm.mTimeChange();
}
void DirectoryNode::removeSubnode(const hash_t& nodeNameHash)
{
    if (subnodeAttr.count(nodeNameHash)) {
        subnodeAttr.erase(nodeNameHash);
        --_refs;
    }
    tm.mTimeChange();
}
hash_t DirectoryNode::calcHash(const string& str)
{
    std::hash<string> hs;
    return (hash_t)(hs(str));
}
void DirectoryNode::removeSubnodeByInum(const inum_t& inodeNum)
{
    hash_t hsh = 0;
    for (auto i : subnodeAttr) {
        if (i.second.addr == inodeNum) {
            hsh = i.first;
            break;
        }
    }
    if (hsh)
        removeSubnode(hsh);
}
ByteArray DirectoryNode::dataExport()
{
    auto tmp = new Byte[subnodeAttr.size() * sizeof(NodeCoreAttr) + sizeof(size_t)];
    int cnt = 0;
    *reinterpret_cast<size_t*>(tmp) = subnodeAttr.size();
    for (auto i : subnodeAttr)
        reinterpret_cast<NodeCoreAttr*>(tmp + sizeof(size_t))[cnt++] = i.second;
    auto rt = ByteArray(sizeof(size_t) + subnodeAttr.size() * sizeof(NodeCoreAttr), tmp);
    delete[] tmp;
    return rt;
}
FileNode::FileNode()
    : FSNode()
{
}
FileNode::FileNode(const FSNode& node)
    : FSNode(node)
{
}
