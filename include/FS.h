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
    void fsInfo() const;
    void fsInit();
    void fsExit();

private:
    HardwareAbstractionLayer HAL;
    Superblock superblock;
    ExtentTree* extentTree;
    InodeMap* inodeMap;

    ByteArray extentAutoRead(const LBA_t& addr);
    FSNode* getInode(const inum_t& inum);
    ByteArray getData(const LBA_t& addr);
    LBA_t largeDataWrite(const ByteArray& data);
    LBA_t smallDataWrite(const ByteArray& data);
    ByteArray smallDataRead(const LBA_t& addr);
    ByteArray largeDataRead(LBA_t addr);
    void extentTreeSave();
};
#endif
