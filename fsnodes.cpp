#include "include/fsnodes.h"
DirectoryNode::DirectoryNode() { access.setDirectory(); }
DirectoryNode::DirectoryNode(const FSNode& node, const ByteArray& ext)
    : FSNode(node)
{
    access.setDirectory();
    if (!ext.length())
        return;
    cerr << "restore directory" << endl;
    //ext.print();
    size_t sz = *reinterpret_cast<size_t*>(ext[0]);
    BytePtr rdPtr = ext[sizeof(size_t)];
    for (int i = 0; i < sz; i++) {
        inum_t addr = *reinterpret_cast<inum_t*>(rdPtr);
        rdPtr += sizeof(inum_t);
        size_t len = *reinterpret_cast<size_t*>(rdPtr);
        rdPtr += sizeof(size_t);
        string name = string(rdPtr, rdPtr + len);
        rdPtr += len;
        subnodeAttr[calcHash(name)] = { addr, name };
    }
    print();
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
        ++_refs;
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
    int cnt = 0, strSize = 0;
    for (auto i : subnodeAttr) {
        strSize += i.second.name.length();
    }
    size_t bufSize = subnodeAttr.size() * (sizeof(inum_t) + sizeof(size_t)) + strSize + sizeof(size_t);
    auto tmp = new Byte[bufSize];
    *reinterpret_cast<size_t*>(tmp) = subnodeAttr.size();
    BytePtr wtPtr = tmp + sizeof(size_t);
    for (auto i : subnodeAttr) {
        *reinterpret_cast<inum_t*>(wtPtr) = i.second.addr;
        wtPtr += sizeof(inum_t);
        *reinterpret_cast<size_t*>(wtPtr) = i.second.name.length();
        wtPtr += sizeof(size_t);
        memcpy(wtPtr, i.second.name.c_str(), i.second.name.length());
        wtPtr += i.second.name.length();
    }
    auto rt = ByteArray(bufSize, tmp);
    cerr << "export directory" << endl;
    rt.print();
    delete[] tmp;
    return rt;
}
FileNode::FileNode()
    : FSNode()
{
    _refs = 1;
}
FileNode::FileNode(const FSNode& node)
    : FSNode(node)
{
}
