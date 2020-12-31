#ifndef FS_H
#define FS_H
#include "HardwareAbstractionLayer.h"
#include "constants.h"
#include "extenttree.h"
#include "fsnode.h"
#include "fsnodes.h"
#include "inodemap.h"
#include "superblock.h"
#include <bits/stdc++.h>
class FS {
public:
    FS();
    void fsInfo() const;
    void fsInit();
    void fsExit();
    void fsMake();
    inline inum_t rootInum() const;
    ByteArray readFile(const inum_t& inum); //can only read accessible file data
    void writeFile(const inum_t& inum, const ByteArray& nwdata); //can only modify accessible file data
    list<NodeCoreAttr> readDirectory(const inum_t& inum); //return the list of subnodes of specific dir-node
    inum_t querySubnodeInum(const inum_t& parent, const string& name); //return the inum of specific subnode under parent-node
    void removeNode(const inum_t& parent, const inum_t& node); //remove the node whether it's a file-node or dir-node
    void createNode(const inum_t& parent, const string& name, const bool& isDirectory); //create a node under 'parent' whose type is specificed by isDirectory, parent must be a dir-node
    inline FSNode getInodeBase(const inum_t& inum);
#ifndef RELEASE
    void test();
#endif
private:
    HardwareAbstractionLayer HAL;
    Superblock superblock;
    ExtentTree* extentTree;
    InodeMap* inodeMap;
    stack<std::pair<ByteArray, std::function<void(LBA_t)>>> writeStack;
    short writeStackSize;

    inline ByteArray extentAutoRead(const LBA_t& addr);
    inline list<pair<LBA_t, LBA_t>> extentHeadAutoRead(LBA_t addr); //return the extents in the specific extent group, first:startpos, second:length, whether smallData or largeData
    FSNode* getInode(const inum_t& inum);
    ByteArray getData(const LBA_t& addr);
    void saveInode(const FSNode& node); //only save the base, if node is a directory, dentrys need to be manually saved
    LBA_t largeDataWrite(const ByteArray& data);
    LBA_t smallDataWrite(const ByteArray& data, const std::function<void(LBA_t)>& callback);
    ByteArray smallDataRead(const LBA_t& addr);
    ByteArray largeDataRead(LBA_t addr);
    pair<LBA_t, LBA_t> largeDataReadHead(const LBA_t& addr); //return the head info of a specific extent, first: length, second: prev
    void extentTreeSave();
    void freeWriteStack(); //free the writeStack, really write data to disk
    void removeNodes(const inum_t& node); //remove subnode recursively without modifying the parent node
};
ByteArray FS::extentAutoRead(const LBA_t& addr)
{
    if (!addr)
        return ByteArray();
    if (addr >> ACTUAL_LBA_BITS)
        return smallDataRead(addr);
    return largeDataRead(addr);
}
inum_t FS::rootInum() const { return superblock.root(); }
FSNode FS::getInodeBase(const inum_t& inum)
{
    if (inum >= FSNode::NodeNull) //bug may occur if processed here
        throw "node not found";
    auto tmp = getInode(inum);
    auto rt = *tmp;
    delete tmp;
    return rt;
}
#endif
