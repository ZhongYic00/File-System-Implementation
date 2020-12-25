#ifndef FSNODES_H
#define FSNODES_H
#include "HardwareAbstractionLayer.h"
#include "fsnode.h"
typedef ull hash_t;
struct NodeCoreAttr {
    LBA_t addr;
    string name;
};

class DirectoryNode : public FSNode {
public:
    DirectoryNode(const FSNode& node, const ByteArray& ext);
    void addSubnode(const LBA_t& addr, const string& nodeName); //add subnode, assuming there's no same nodeName
    void removeSubnodeByInum(const inum_t& inodeNum); //remove subnode by inodeNumber
    void removeSubnode(const hash_t& nodeNameHash); //remove subnode by nodeName(hash)
    inline void removeSubnode(const string& nodeName); //remove subnode by nodeName

protected:
    static hash_t calcHash(const string& str);

private:
    map<hash_t, NodeCoreAttr> subnodeAttr; //key => hash-value of name
};
class FileNode : public FSNode {
public:
    FileNode();

protected:
private:
    size_t _length;
};

inline void DirectoryNode::removeSubnode(const string& nodeName)
{
    this->removeSubnode(calcHash(nodeName));
}
#endif //FSNODES_H
