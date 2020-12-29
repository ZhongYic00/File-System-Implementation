#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H
#include "constants.h"
#include "fsnode.h"
class Superblock {
public:
    inline LBA_t root() const;
    inline LBA_t extentTreeLBA() const;
    inline LBA_t extentTreeBakLBA() const;
    inline LBA_t extentTreeRendLBA() const;
    inline LBA_t freeInumPoolLBA() const;
    inline LBA_t inodeMapLBA() const;
    inline void updateExtentTreeLBA(const LBA_t& lba);
    inline void updateExtentTreeRendLBA(const LBA_t& lba);
    inline void updateInodeMapLBA(const LBA_t& lba);
    inline inum_t inumCnt() const;
    inline bool invalid() const;
};
#endif
