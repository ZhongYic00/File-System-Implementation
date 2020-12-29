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
FileNode::FileNode()
    : FSNode()
{
}
FileNode::FileNode(const FSNode& node)
    : FSNode(node)
{
}
