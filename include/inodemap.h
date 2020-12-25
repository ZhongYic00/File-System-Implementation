#ifndef INODEMAP_H
#define INODEMAP_H
#include "constants.h"
#include "fsnode.h"
class InodeMap {
public:
    InodeMap(const ByteArray& d);
    ByteArray dataExport() const;
    LBA_t queryLBA(const inum_t i) const;
};
#endif
