#include "include/fsnodes.h"
void DirectoryNode::updateTotalSize()
{
    _totalSize = metaSize() + subnodes.size();
}
void DirectoryNode::updateSubnodes()
{
}
