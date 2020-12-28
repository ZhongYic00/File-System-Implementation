#ifndef CONSTANTS_H
#define CONSTANTS_H
#define KB (1 << 10)
#define MB (KB << 10)
#define GB (MB << 10)
#define BLOCKSIZE_KB (4)
#define BLOCKSIZE_BYTE (BLOCKSIZE_KB * KB)
#define DEVICE_SIZE (5 * GB)
#define DEVICE_SIZE_BLK (DEVICE_SIZE / BLOCKSIZE_BYTE)
#define SUPERBLOCK_LBA 0
#define BAK_SUPERBLOCK_LBA DEVICE_SIZE_BLK
#define SUPERBLOCK_REAL_SIZE sizeof(Superblock)
#define SUPERBLOCK_SIZE_BLK 1
#define SUPERBLOCK_SIZE_BYTE (SUPERBLOCK_SIZE_BLK * 4 * KB)

typedef unsigned char u8;
typedef u8 Byte;
typedef Byte* BytePtr;
typedef unsigned long long LBA_t;
#endif // CONSTANTS_H
