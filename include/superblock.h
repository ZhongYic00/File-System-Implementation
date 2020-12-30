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
    inline bool isValid() const;

private:
    LBA_t _root;
    LBA_t _extentTreeLBA;
    LBA_t _extentTreeBakLBA;
    LBA_t _extentTreeRendLBA;
    LBA_t _freeInumPoolLBA;
    LBA_t _inodeMapLBA;
    inum_t _inumCnt;
    bool valid;
};

LBA_t Superblock::root() const { return _root; }
LBA_t Superblock::extentTreeLBA() const { return _extentTreeLBA; }
LBA_t Superblock::extentTreeBakLBA() const { return _extentTreeBakLBA; }
LBA_t Superblock::extentTreeRendLBA() const { return _extentTreeRendLBA; }
LBA_t Superblock::freeInumPoolLBA() const { return _freeInumPoolLBA; }
LBA_t Superblock::inodeMapLBA() const { return _inodeMapLBA; }
void Superblock::updateExtentTreeLBA(const LBA_t& lba) { _extentTreeLBA = lba; }
void Superblock::updateExtentTreeRendLBA(const LBA_t& lba) { _extentTreeRendLBA = lba; }
void Superblock::updateInodeMapLBA(const LBA_t& lba) { _inodeMapLBA = lba; }
inum_t Superblock::inumCnt() const { return _inumCnt; }
bool Superblock::isValid() const { return valid; }
#endif
