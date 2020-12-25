#include "include/FS.h"
void FS::fsInfo() const
{
    printf("FileSystem Size: %ullBytes\n", HAL.deviceSize());
}
void FS::fsInit()
{
    //load superblock
    Byte* superblockTmp = new Byte[SUPERBLOCK_SIZE_BYTE];
    HAL.read(SUPERBLOCK_LBA, SUPERBLOCK_SIZE_BLK, superblockTmp);
    memcpy(&superblock, superblockTmp, SUPERBLOCK_REAL_SIZE);
    if (superblock.invalid()) {
        HAL.read(BAK_SUPERBLOCK_LBA, SUPERBLOCK_SIZE_BLK, superblockTmp);
        if (superblock.invalid()) {
            exit(1);
        }
        //recover main-superblock via backup superblock
        memset(superblockTmp, 0, SUPERBLOCK_SIZE_BYTE);
        memcpy(superblockTmp, &superblock, SUPERBLOCK_REAL_SIZE);
        HAL.write(SUPERBLOCK_LBA, superblockTmp, SUPERBLOCK_SIZE_BLK);
    }
    delete[] superblockTmp;

    //load extent-tree
    auto extentTreeBuildTmp = extentAutoRead(superblock.extentTreeLBA());
    extentTree = new ExtentTree(extentTreeBuildTmp);

    //load inode-map
    auto inodeMapBuildTmp = extentAutoRead(superblock.inodeMapLBA());
    inodeMap = new InodeMap(inodeMapBuildTmp);

    //load freeInumPool
    auto inumPoolTmp = extentAutoRead(superblock.freeInumPoolLBA());
    inum_t* inumPoolArrayEnd = reinterpret_cast<inum_t*>(inumPoolTmp.end() - 4);
    for (inum_t* i = reinterpret_cast<inum_t*>(inumPoolTmp.begin()); i < inumPoolArrayEnd; i++) {
        //如何向private类变量加入数据？暂时用友元类解决
        FSNode::freeInum.push(*i);
    }
    //load inumCnt
    FSNode::nodeCount = superblock.inumCnt();
}
void FS::fsExit()
{
    //write the rest small pieces to disk

    //save inode-map
    LBA_t inodeMapLBA = largeDataWrite(inodeMap->dataExport());
    superblock.updateInodeMapLBA(inodeMapLBA);

    //save extent-tree
    extentTreeSave();

    //save superblock
    Byte* superblockTmp = new Byte[SUPERBLOCK_SIZE_BYTE];
    memset(superblockTmp, 0, SUPERBLOCK_SIZE_BYTE);
    memcpy(superblockTmp, &superblock, SUPERBLOCK_REAL_SIZE);
    HAL.write(SUPERBLOCK_LBA, superblockTmp, SUPERBLOCK_SIZE_BLK);
}
void FS::extentTreeSave()
{
    //save the rest logs

    /*extentTree layout on disk is as this:
     *  | superblock | extent log area | extentTreeBak(version N-1) | extentTree(version N) |
     *                                 ^                            ^                       ^
     *                           extentTreeBakLBA           extentTreeLBA           extentTreeRendLBA
     */
    //move extent-tree(n)
    LBA_t extentTreeRbegin = superblock.extentTreeLBA();
    LBA_t extentTreeBakRbegin = superblock.extentTreeBakLBA();
    const LBA_t extentTreeRend = superblock.extentTreeRendLBA();
    BytePtr blkTmp = new Byte[BLOCKSIZE_BYTE];
    for (; extentTreeRbegin >= extentTreeRend; extentTreeRbegin--, extentTreeBakRbegin--) {
        HAL.read(extentTreeRbegin, 1, blkTmp);
        HAL.write(extentTreeBakRbegin, blkTmp, 1);
    }
    delete[] blkTmp;
    superblock.updateExtentTreeLBA(++extentTreeBakRbegin);

    //save extent-tree(n+1)
    ByteArray extentTreeTmp = extentTree->dataExport();
    HAL.write(superblock.extentTreeLBA(), extentTreeTmp.begin(), extentTreeTmp.length());
    superblock.updateExtentTreeRendLBA(extentTreeBakRbegin - extentTreeTmp.length());
}
FSNode* FS::getInode(const inum_t& inum)
{
    LBA_t inodeLBA = inodeMap->queryLBA(inum);
    FSNode nodeTmp(smallDataRead(inodeLBA));
    if (nodeTmp.invalid())
        return new FSNode(FSNode::NodeInvalid);
    if (!nodeTmp.isDirectory())
        return new FSNode(nodeTmp);
    LBA_t dirAttrLBA = nodeTmp.dataExtentLBA();
    return new DirectoryNode(nodeTmp, extentAutoRead(dirAttrLBA));
}
ByteArray FS::getData(const LBA_t& addr)
{
    return extentAutoRead(addr);
}
LBA_t FS::smallDataWrite(const ByteArray& data) {}
LBA_t FS::largeDataWrite(const ByteArray& data)
{
    auto estimateBlocks = [](size_t size) -> LBA_t {
        //bit_count(cell((size+blks*64*2)/4096))=blks
    };
    auto newExtent = [](LBA_t prev, BytePtr st, size_t len) -> BytePtr {};
    auto extents = extentTree->allocateExtents(estimateBlocks(data.length() >> 12)); //convert bytes to blocks
    LBA_t prev = 0;
    size_t pos = 0;
    for (auto i : extents) {
        auto extent = newExtent(prev, data[pos], i.second); //extent is a continous memory segment which stores the next_LBA ptr at head, followed by size(blocks) of the extent, and data after that
        HAL.write(i.first, reinterpret_cast<BytePtr>(extent), i.second);
        delete extent;
        prev = i.first;
        pos += i.second - 1;
    }
    return prev;
}
ByteArray FS::smallDataRead(const LBA_t& addr)
{
    auto tmp = new Byte[BLOCKSIZE_BYTE];
    auto standerizeLBA = [](const LBA_t& x) -> LBA_t {
        return x & ((1ULL << 48) - 1);
    };
    HAL.read(standerizeLBA(addr), 1, tmp);
    unsigned long long bitmap[4] = { reinterpret_cast<unsigned int*>(tmp)[0], reinterpret_cast<unsigned int*>(tmp)[1], reinterpret_cast<unsigned int*>(tmp)[2], reinterpret_cast<unsigned int*>(tmp)[3] };
    auto findPieces = [&bitmap](u8 index) {
        for (int k = 0, j = 0, b = 0; k < 3; k++) { //bugs may occur
            unsigned long long tmp = bitmap[k];
            for (int i = 0; i < 64;) {
                while ((tmp ^ static_cast<unsigned long long>(b)) & 1ULL) { //bugs may occur
                    i++;
                    tmp >>= 1;
                }
                j++;
                if (j >= index)
                    return i - 1 + (k << 6);
                b ^= 1;
            }
        }
        return -1;
    }; //return the exact end pos of piece index in an block e.g. findPieces(0)=1, which means the 1st piece occupies (16B) piece 0 and 1
    int startPos = findPieces(static_cast<u8>(addr >> 48) - 1) + 1; // >>48 cuts high 8 bits to locate pieces in the block
    int len = findPieces(static_cast<u8>(addr >> 48)) - startPos + 1;
    auto rt = ByteArray(static_cast<size_t>(len << 4), tmp + (startPos << 4));
    delete[] tmp;
    return rt;
}
ByteArray FS::largeDataRead(LBA_t addr)
{
    auto tmp = new Byte[BLOCKSIZE_BYTE];
    list<ByteArray> buf;
    do {
        HAL.read(addr, 1, tmp);
        auto blks = reinterpret_cast<LBA_t*>(tmp)[1];
        auto ttmp = new Byte[BLOCKSIZE_BYTE * blks];
        HAL.read(addr, blks, ttmp);
        buf.push_back(ByteArray(BLOCKSIZE_BYTE * blks, ttmp));
        delete[] ttmp;
        addr = reinterpret_cast<LBA_t*>(tmp)[0];
    } while (addr);
    delete[] tmp;
    return ByteArray(buf);
}
