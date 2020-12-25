#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H
#include "constants.h"
#include "fsnode.h"
class Superblock {
public:
    LBA_t extentTreeLBA() const;
    LBA_t extentTreeBakLBA() const;
    LBA_t extentTreeRendLBA() const;
    LBA_t freeInumPoolLBA() const;
    LBA_t inodeMapLBA() const;
    void updateExtentTreeLBA(const LBA_t& lba);
    void updateExtentTreeRendLBA(const LBA_t& lba);
    void updateInodeMapLBA(const LBA_t& lba);
    inum_t inumCnt() const;
    bool invalid() const;
};
#endif
