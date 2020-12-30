#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H
#include "constants.h"
#include "fsnode.h"
class Superblock {
public:
    Superblock()
        : _root(ROOT_INUM)
        , _inumCnt(1)
        , valid(true)
    {
        _extentTreeBakLBA = DEVICE_SIZE_BLK - SUPERBLOCK_SIZE_BLK - EXTENT_LOG_SIZE_BLK;
        _extentTreeRendLBA = _extentTreeLBA = _extentTreeBakLBA;
        _inodeMapLBA = 0;
    }
    inline LBA_t root() const;
    inline LBA_t extentTreeLBA() const;
    inline LBA_t extentTreeBakLBA() const;
    inline LBA_t extentTreeRendLBA() const;
    inline LBA_t freeInumPoolLBA() const;
    inline LBA_t inodeMapLBA() const;
    inline void updateExtentTreeLBA(const LBA_t& lba);
    inline void updateExtentTreeRendLBA(const LBA_t& lba);
    inline void updateInodeMapLBA(const LBA_t& lba);
    inline void updateInumCnt(const inum_t& inum);
    inline void updateFreeInumPoolLBA(const LBA_t& lba);
    inline inum_t inumCnt() const;
    inline bool isValid() const;
    inline bool empty() const;

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
void Superblock::updateInumCnt(const inum_t& inum) { _inumCnt = inum; }
void Superblock::updateFreeInumPoolLBA(const LBA_t& lba) { _freeInumPoolLBA = lba; }
inum_t Superblock::inumCnt() const { return _inumCnt; }
bool Superblock::isValid() const { return valid; }
bool Superblock::empty() const
{
    for (int i = 0; i < sizeof(*this); i++)
        if (reinterpret_cast<const char*>(this)[i])
            return false;
    return true;
}
#endif
