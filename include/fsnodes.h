#ifndef FSNODES_H
#define FSNODES
#include "HardwareAbstractionLayer.h"
#include "fsnode.h"
class DirectoryNode : public FSNode {
public:
protected:
    void updateBodySize();
    void updateTotalSize();
    void updateSubnodes();

private:
    map<ull, LBA_t> subnodes;
};
class FileNode : public FSNode {
};
#endif //FSNODES_H
