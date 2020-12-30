#include "include/fsnodes.h"
DirectoryNode::DirectoryNode() {}
DirectoryNode::DirectoryNode(const FSNode& node, const ByteArray& ext)
    : FSNode(node)
{
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
    return (hash_t)(0);
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
    auto tmp = new NodeCoreAttr[subnodeAttr.size()];
    int cnt = 0;
    for (auto i : subnodeAttr)
        tmp[cnt++] = i.second;
    auto rt = ByteArray(subnodeAttr.size() * sizeof(NodeCoreAttr), reinterpret_cast<BytePtr>(tmp));
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
