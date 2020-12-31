#define FUSE_USE_VERSION 31

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
#define _XOPEN_SOURCE 700
#endif

#include "include/FS.h"
#include "include/constants.h"
#include <bits/stdc++.h>
#include <fuse3/fuse.h>
using namespace std;

static FS fs;

static inum_t parse(const char* path, inum_t& father, string& name)
{
    cerr << "call parse " << path << endl;
    inum_t inum = 0;
    int len = strlen(path);
    father = fs.rootInum();
    string now = "";
    for (int i = 0; i < len; i++) {
        if (now == "")
            inum = father;
        if (path[i] == '/' || i == len - 1) {
            if (now != "") {
                try {
                    inum = fs.querySubnodeInum(father, now);
                } catch (string s) {
                    cerr << s << endl;
                    if (i != len - 1)
                        return static_cast<ull>(-1);
                    else {
                        name = now;
                        inum = father;
                        break;
                    }
                }
                now = "";
            }
            continue;
        }
        now += path[i];
    }
    return inum;
}

static void* fs_init(struct fuse_conn_info* conn, struct fuse_config* cfg)
{
    cerr << "call fs_init" << endl;
    fs.fsInit();
    return nullptr;
}
static void fs_destroy(void* private_data)
{
    fs.fsExit();
}
static int fs_statfs(const char* path, struct statvfs* stbuf)
{
    stbuf->f_bsize = 4096;
    return 0;
}
static int fs_getattr(const char* path, struct stat* stbuf, struct fuse_file_info* fi)
{
    cerr << "call fs_getattr" << endl;
    stbuf->st_mode = S_IFDIR | 0777;
    stbuf->st_size = 4096;
    stbuf->st_nlink = 1;
    stbuf->st_uid = 0;
    stbuf->st_gid = 0;
    stbuf->st_rdev = 0;
    return 0;
}
static int fs_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags)
{
    cerr << "call fs_readdir" << endl;
    string s = "";
    inum_t fa = 0;
    struct stat st;
    auto file = parse(path, fa, s);
    cerr << "get inum " << file << endl;
    list<NodeCoreAttr> ls;
    try {
        ls = fs.readDirectory(file);
    } catch (string s) {
        cerr << s << endl;
        return -1;
    }
    cerr << "subnodes: ";
    for (const auto& i : ls) {
        cerr << i.name << ' ';
        memset(&st, 0, sizeof(st));
        st.st_mode = (fs.getInodeBase(i.addr).isDirectory() ? S_IFDIR : S_IFMT);
        if (filler(buf, i.name.c_str(), &st, 0, fuse_fill_dir_flags(0)))
            break;
    }
    cerr << endl;
    return 0;
}
static int fs_open(const char* path, struct fuse_file_info* fi)
{
    return 0;
}
static int fs_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
    //access judgement, using fi
    if (offset < 0)
        return -1;
    string s = "";
    inum_t fa = 0;
    auto tmp = fs.readFile(parse(path, fa, s));
    memcpy(buf, tmp[offset], size);
    return size;
}
static int fs_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
    //access judgement, using fi
    if (offset < 0)
        return -1;
    string s = "";
    inum_t fa = 0;
    auto file = parse(path, fa, s);
    auto tmp = fs.readFile(file);
    memcpy(tmp[offset], buf, size);
    fs.writeFile(file, tmp);
    return 0;
}
static int fs_access(const char* path, int)
{
    return 0;
}
static int fs_chmod(const char* path, mode_t mod, struct fuse_file_info* fi)
{
    return 0;
}
static int fs_mknod(const char* path, mode_t mod, dev_t dev)
{
    string name = "";
    inum_t fa = 0;
    auto file = parse(path, fa, name);
    if (name == "") {
        throw "No file name decected.";
        return -1;
    }
    try {
        fs.createNode(file, name, 0);
    } catch (string s) {
        return -1;
    }
    return 0;
}
static int fs_mkdir(const char* path, mode_t mod)
{
    string name = "";
    inum_t fa = 0;
    auto file = parse(path, fa, name);
    if (name == "") {
        throw "No directory name detected.";
        return -1;
    }
    try {
        fs.createNode(file, name, 1);
    } catch (string s) {
        return -1;
    }
    return 0;
}
static int fs_rmnod(const char* path)
{
    string s = "";
    inum_t father = 0;
    auto file = parse(path, father, s);
    try {
        fs.removeNode(father, file);
    } catch (string s) {
        return -1;
    }
    return 0;
}
static int fs_rmdir(const char* path)
{
    string s = "";
    inum_t father = 0;
    auto file = parse(path, father, s);
    try {
        fs.removeNode(father, file);
    } catch (string s) {
        return -1;
    }
    return 0;
}
static struct fuse_operations fs_operations = {
    .getattr = fs_getattr,
    .mknod = fs_mknod,
    .mkdir = fs_mkdir,
    .unlink = fs_rmnod,
    .rmdir = fs_rmdir,
    .chmod = fs_chmod,
    .open = fs_open,
    .read = fs_read,
    .readdir = fs_readdir,
    .init = fs_init,
    .destroy = fs_destroy,
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
