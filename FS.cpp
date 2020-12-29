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
    FSNode nodeBase(smallDataRead(inodeLBA));
    if (nodeBase.isValid())
        return new FSNode(FSNode::NodeInvalid);
    if (!nodeBase.isDirectory())
        return new FileNode(nodeBase);
    LBA_t dirAttrLBA = nodeBase.dataExtentLBA();
    return new DirectoryNode(nodeBase, extentAutoRead(dirAttrLBA));
}
ByteArray FS::getData(const LBA_t& addr)
{
    return extentAutoRead(addr);
}
void FS::saveInode(const FSNode& node)
{
    inum_t inum = node.inum();
    smallDataWrite(node.nodeDataExport(), [&](LBA_t addr) -> void {
        LBA_t ori = inodeMap->queryLBA(inum);
        if (ori != 0) {
            //delete the origin inode
            extentTree->releaseExtent(ori);
        }
        inodeMap->updateLBA(inum, addr); //problems may occur since inodemap may have been destructed when this function called
    });
}
LBA_t FS::smallDataWrite(const ByteArray& data, const std::function<void(LBA_t)>& callback)
{
    //write delay
    auto pieces = [](size_t sz) {
        return (sz >> 4) + sz & 15;
    }; //count how many pieces is needed
    u8 pcs = pieces(data.length());
    if (writeStackSize + pcs > USABLE_PIECES_PER_BLOCK)
        freeWriteStack();
    writeStack.push({ data, callback });
    writeStackSize += pcs;
}
LBA_t FS::largeDataWrite(const ByteArray& data) //数据组织方向好像反了
{
    const size_t HEAD_RESERVED = sizeof(LBA_t) + sizeof(u8);
    auto estimateBlocks = [](size_t size) -> LBA_t {
        //bit_count(cell((size+blks*64+blks*8)/4096))=blks
    };
    auto newExtent = [](LBA_t prev, BytePtr st, u8 blks_k) -> BytePtr {
        auto rt = new Byte[(1ULL << blks_k) * BLOCKSIZE_BYTE];
        memcpy(rt + HEAD_RESERVED, st, (1ULL << blks_k) * BLOCKSIZE_BYTE - HEAD_RESERVED);
        *reinterpret_cast<LBA_t*>(rt) = prev;
        *reinterpret_cast<u8*>(rt + sizeof(LBA_t)) = blks_k;
        return rt;
    };
    auto extents = extentTree->allocateExtents(estimateBlocks(data.length() >> 12)); //convert bytes to blocks, extentTree only cares about block
    //bug here, assume that there's a very small piece, smaller than a block, invalid visit may happen when copying data into write buffer
    LBA_t prev = 0;
    size_t pos = 0;
    for (auto i : extents) {
        auto extent = newExtent(prev, data[pos], i.second); //extent is a continous memory segment which stores the next_LBA ptr at head, followed by size of the extent(measured by block, use 2^k to represent), and data after that
        HAL.write(i.first, reinterpret_cast<BytePtr>(extent), 1ULL << i.second);
        pos += (1ULL << i.second) * BLOCKSIZE_BYTE - HEAD_RESERVED;
        delete[] extent;
        prev = i.first;
    }
    return prev;
}
ByteArray FS::smallDataRead(const LBA_t& addr)
{
    auto tmp = new Byte[BLOCKSIZE_BYTE];
    auto standerizeLBA = [](const LBA_t& x) -> LBA_t {
        return x & ((1ULL << ACTUAL_LBA_BITS) - 1);
    };
    HAL.read(standerizeLBA(addr), 1, tmp);
    unsigned long long bitmap[4] = { reinterpret_cast<unsigned int*>(tmp)[0], reinterpret_cast<unsigned int*>(tmp)[1], reinterpret_cast<unsigned int*>(tmp)[2], reinterpret_cast<unsigned int*>(tmp)[3] };
    auto findPieces = [&bitmap](u8 index) -> LBA_t {
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
    LBA_t startPos = findPieces(static_cast<u8>(addr >> ACTUAL_LBA_BITS) - 1) + 1; // >>ACTUAL_LBA_BITS cuts high 8 bits to locate pieces in the block
    LBA_t len = findPieces(static_cast<u8>(addr >> ACTUAL_LBA_BITS)) - startPos + 1;
    auto rt = ByteArray(static_cast<size_t>(len * BLOCKSIZE_BYTE), tmp + (startPos * BLOCKSIZE_BYTE));
    delete[] tmp;
    return rt;
}
ByteArray FS::largeDataRead(LBA_t addr)
{
    const size_t HEAD_RESERVED = sizeof(LBA_t) + sizeof(u8);
    auto tmp = new Byte[BLOCKSIZE_BYTE];
    list<ByteArray> buf;
    do {
        HAL.read(addr, 1, tmp);
        auto blks = 1ULL << (*reinterpret_cast<u8*>(tmp + sizeof(LBA_t)));
        auto rdbuf = new Byte[BLOCKSIZE_BYTE * blks];
        HAL.read(addr, blks, rdbuf);
        buf.push_back(ByteArray(BLOCKSIZE_BYTE * blks - HEAD_RESERVED, rdbuf + HEAD_RESERVED));
        delete[] rdbuf;
        addr = *reinterpret_cast<LBA_t*>(tmp);
    } while (addr);
    delete[] tmp;
    return ByteArray(buf);
}
pair<LBA_t, LBA_t> FS::largeDataReadHead(const LBA_t& addr)
{
    auto tmp = new Byte[BLOCKSIZE_BYTE];
    HAL.read(addr, 1, tmp);
    auto blks = 1ULL << (*reinterpret_cast<u8*>(tmp + sizeof(LBA_t)));
    pair<LBA_t, LBA_t> rt = { blks, *reinterpret_cast<LBA_t*>(tmp) };
    delete[] tmp;
    return rt;
}
list<pair<LBA_t, LBA_t>> FS::extentHeadAutoRead(LBA_t addr)
{
    auto isLarge = [](const LBA_t& x) {
        return static_cast<bool>(x >> ACTUAL_LBA_BITS);
    };
    list<pair<LBA_t, LBA_t>> rt;
    do {
        if (isLarge(addr)) {
            auto cur = largeDataReadHead(addr);
            rt.push_back({ addr, cur.first });
            addr = cur.second;
        } else {
            rt.push_back({ addr, 1 });
            addr = 0;
        }
    } while (addr);
}
ByteArray FS::readFile(const inum_t& inum)
{
    FSNode inode(extentAutoRead(inodeMap->queryLBA(inum)));
    if (inode.isValid()) {
        throw "inode read error";
        return ByteArray();
    }
    if (inode.isDirectory()) {
        throw "not a file node";
        return ByteArray();
    }
    //access judgement

    return extentAutoRead(inode.dataExtentLBA());
}
void FS::writeFile(const inum_t& inum, const ByteArray& nwdata)
{
    FSNode inode(extentAutoRead(inodeMap->queryLBA(inum)));
    if (inode.isValid()) {
        throw "inode read error";
        return;
    }
    if (inode.isDirectory()) {
        throw "not a file node";
        return;
    }
    //access judgement

    extentTree->releaseExtent(extentHeadAutoRead(inode.dataExtentLBA()));
    inode.updateDataExtentLBA(largeDataWrite(nwdata));
    saveInode(inode);
}
void FS::removeNode(const inum_t& parent, const inum_t& node)
{
    //what if there're subnodes under 'node' ?
    auto prnt = getInode(parent);
    if (prnt->isValid()) {
        throw "inode read error";
        return delete prnt;
    }
    if (!prnt->isDirectory()) { //in case of illegal call
        throw "not a directory node";
        return delete prnt;
    }
    removeNodes(node);
    dynamic_cast<DirectoryNode*>(prnt)->removeSubnodeByInum(node);
    saveInode(*prnt);
    delete prnt;
}
void FS::removeNodes(const inum_t& node)
{
    //release subnodes
    auto nd = getInode(node);
    if (nd->isValid()) {
        throw "inode read error";
        return delete nd;
    }
    //if nd is a dir, recursively remove all subnodes. elif nd is a file, just reduce the reference count.
    if (nd->isDirectory()) {
        for (auto subnd : readDirectory(node)) {
            removeNodes(subnd.addr);
            dynamic_cast<DirectoryNode*>(nd)->removeSubnodeByInum(subnd.addr);
        }
        //dynamic_cast<DirectoryNode*>(nd)->resetRef();
    } else {
        dynamic_cast<FileNode*>(nd)->reduceRef();
    }
    //check reference
    if (nd->refCount() == 0) {
        //if there's no reference, free the space

        //release data
        extentTree->releaseExtent(extentHeadAutoRead(nd->dataExtentLBA())); //standerization is processed in extentTree func
        //release meta-data
        extentTree->releaseExtent(inodeMap->queryLBA(node), 1);
        inodeMap->remove(node);
    } else {
        //here nd can only be a file-node, otherwise something must be wrong
        saveInode(*nd);
    }
    delete nd;
}

void FS::createNode(const inum_t& parent, const string& name, const bool& isDirectory)
{
    auto ori = getInode(parent);
    if (!ori->isDirectory()) {
        throw "not a directory node";
        return delete ori;
    }
    FSNode* nwNode = nullptr;
    if (isDirectory)
        nwNode = new DirectoryNode;
    else
        nwNode = new FSNode;
    dynamic_cast<DirectoryNode*>(ori)->addSubnode(nwNode->inum(), name);
    //save dentry
    ori->updateDataExtentLBA(
        largeDataWrite(dynamic_cast<DirectoryNode*>(ori)->dataExport()));
    //save node
    saveInode(*ori);
    saveInode(*nwNode);
    delete ori;
    delete nwNode;
}
