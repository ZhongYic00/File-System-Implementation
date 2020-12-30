#ifndef FSNODES_H
#define FSNODES_H
#include "HardwareAbstractionLayer.h"
#include "fsnode.h"
typedef ull hash_t;
struct NodeCoreAttr {
    inum_t addr;
    string name;
};

class DirectoryNode : public FSNode {
public:
    DirectoryNode();
    DirectoryNode(const FSNode& node, const ByteArray& ext);
    void addSubnode(const inum_t& addr, const string& nodeName); //add subnode, assuming there's no same nodeName
    void removeSubnodeByInum(const inum_t& inodeNum); //remove subnode by inodeNumber
    void removeSubnode(const hash_t& nodeNameHash); //remove subnode by nodeName(hash)
    inline void removeSubnode(const string& nodeName); //remove subnode by nodeName
    inline inum_t getSubnode(const string& nodeName);
    inline list<NodeCoreAttr> readDir() const;
    //inline void resetRef();
    ByteArray dataExport();

protected:
    static hash_t calcHash(const string& str);

private:
    map<hash_t, NodeCoreAttr> subnodeAttr; //key => hash-value of name
};
class FileNode : public FSNode {
public:
    FileNode();
    FileNode(const FSNode& node);
    inline void reduceRef();

protected:
private:
    //size_t _length;
};

void DirectoryNode::removeSubnode(const string& nodeName) { this->removeSubnode(calcHash(nodeName)); }
//void DirectoryNode::resetRef() { _subnodes = 0; })
void FileNode::reduceRef() { --_refs; }
inum_t DirectoryNode::getSubnode(const string& nodeName)
{
    if (subnodeAttr.count(calcHash(nodeName)))
        return subnodeAttr[calcHash(nodeName)].addr;
    else
        throw "node not found!";
}
list<NodeCoreAttr> DirectoryNode::readDir() const
{
    list<NodeCoreAttr> rt;
    for (auto i : subnodeAttr) {
        rt.push_back(i.second);
    }
    return rt;
}
#endif //FSNODES_H
