#include "include/inodemap.h"

using namespace std;

InodeMap::InodeMap(const ByteArray& d)
{
    //Function body here
}
//Notes here

ByteArray InodeMap::dataExport() const
{
    //Function body here
}
//Notes here

LBA_t InodeMap::queryLBA(const inum_t& i) const
{
    //Function body here
}
//Notes here

void InodeMap::updateLBA(const inum_t &i, const LBA_t &addr)
{
    //Function body here
}
//Notes here

void InodeMap::remove(const inum_t &i)
{
    //Function body here
}
//Notes here
