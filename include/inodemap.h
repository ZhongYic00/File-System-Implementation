#ifndef INODEMAP_H
#define INODEMAP_H
#include "constants.h"
#include "fsnode.h"
class InodeMap {
public:
    InodeMap(const ByteArray& d);
    ByteArray dataExport() const;
    LBA_t queryLBA(const inum_t& i) const;
    void updateLBA(const inum_t& i, const LBA_t& addr); //update record if i exists, else insert the record
    void remove(const inum_t& i); //remove the record
};
#endif
