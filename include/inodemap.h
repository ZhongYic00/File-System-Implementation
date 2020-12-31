#ifndef INODEMAP_H
#define INODEMAP_H
#include "constants.h"
#include "fsnode.h"
class InodeMap {
public:
    InodeMap(const ByteArray& d);
    ByteArray dataExport();
    inline LBA_t queryLBA(const inum_t& i);
    inline void updateLBA(const inum_t& i, const LBA_t& addr); //update record if i exists, else insert the record
    inline void remove(const inum_t& i); //remove the record
private:
    std::map<inum_t, LBA_t> inodes;
};
LBA_t InodeMap::queryLBA(const inum_t& i)
{
    if (inodes.count(i))
        return inodes[i];
    return 0;
    //throw "inode not found";
}
void InodeMap::remove(const inum_t& i)
{
    if (inodes.count(i))
        inodes.erase(i);
    throw "inode not found";
}
void InodeMap::updateLBA(const inum_t& i, const LBA_t& addr) { inodes[i] = addr; }
#endif
