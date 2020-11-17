#define FUSE_USE_VERSION 31

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
#define _XOPEN_SOURCE 700
#endif

#include <bits/stdc++.h>
#include <fuse3/fuse.h>

static void* fs_init(struct fuse_conn_info* conn, struct fuse_config* cfg)
{
    return nullptr;
}
static int fs_statfs(const char* path, struct statvfs* stbuf)
{
    stbuf->f_bsize = 4096;
    return 0;
}
static int fs_getattr(const char* path, struct stat* stbuf, struct fuse_file_info* fi)
{
    stbuf->st_mode = S_IFDIR | 0666;
    stbuf->st_size = 4096;
    stbuf->st_nlink = 1;
    stbuf->st_uid = 0;
    stbuf->st_gid = 0;
    stbuf->st_rdev = 0;
    return 0;
}
static int fs_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags)
{
    filler(buf, "hellofs", NULL, 0, fuse_fill_dir_flags(0));
    return 0;
}
static int fs_open(const char* path, struct fuse_file_info* fi)
{
    return 0;
}
static int fs_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
    memset(buf, 0, size);
    return size;
}
static int fs_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
    return 0;
}
static int fs_access(const char* path, int)
{
    return 0;
}
static struct fuse_operations fs_operations = {
    .getattr = fs_getattr,
    .open = fs_open,
    .read = fs_read,
    .readdir = fs_readdir,
    .init = fs_init,
    .access = fs_access, /*
	.mknod = nullptr,
	.mkdir = nullptr,
	.unlink = nullptr,
	.rmdir = nullptr,
	.rename = nullptr,
	.chmod = nullptr,
	.chown = nullptr,
	.truncate = nullptr,
	.open =nullptr,
	.read = nullptr,
	.write = nullptr,
	.statfs = fs_statfs,*/
};
