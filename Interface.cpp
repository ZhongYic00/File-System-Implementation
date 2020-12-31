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
    string pth = string(path);
    inum_t inum = 0;
    int len = pth.length();
    if (pth[len - 1] != '/') {
        pth += '/';
        len++;
    }
    father = fs.rootInum();
    string now = "";
    for (int i = 0; i < len; i++) {
        if (now == "")
            father = inum;
        if (pth[i] == '/') {
            if (now != "") {
                try {
                    inum = fs.querySubnodeInum(father, now);
                    if (inum == FSNode::NodeNull)
                        return inum;
                } catch (const char* s) {
                    cerr << s << endl;
                    if (i != len - 1)
                        return FSNode::NodeInvalid;
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
        now += pth[i];
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
    cerr << endl
         << "call fs_getattr" << endl;
    string s = "";
    inum_t fa = 0;
    struct stat st;
    try {
        auto file = parse(path, fa, s);
        if (s != "")
            return -2;
        stbuf->st_mode = 0777 | (fs.getInodeBase(file).isDirectory() ? (S_IFDIR) : (S_IFREG));
    } catch (const char* s) {
        cerr << s << endl;
        stbuf->st_mode = 0;
        return -2;
    }
    stbuf->st_size = 4096;
    stbuf->st_nlink = 1;
    stbuf->st_uid = 0;
    stbuf->st_gid = 0;
    stbuf->st_rdev = 0;
    return 0;
}
static int fs_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags)
{
    cerr << endl
         << "call fs_readdir" << endl;
    string s = "";
    inum_t fa = 0;
    struct stat st;
    auto file = parse(path, fa, s);
    cerr << "get inum " << file << endl;
    list<NodeCoreAttr> ls;
    try {
        ls = fs.readDirectory(file);
    } catch (const char* s) {
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
    cerr << endl
         << "call fs_read " << path << ' ' << size << ' ' << offset << endl;
    //access judgement, using fi
    if (offset < 0)
        return -1;
    string s = "";
    inum_t fa = 0;
    auto tmp = fs.readFile(parse(path, fa, s));
    if (!tmp.length())
        return 0;
    memcpy(buf, tmp[offset], size);
    return size;
}
static int fs_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
    cerr << endl
         << "call fs_write " << path << ' ' << size << ' ' << offset << endl;
    //access judgement, using fi
    if (offset < 0)
        return -1;
    string s = "";
    inum_t fa = 0;
    auto file = parse(path, fa, s);
    auto ori = fs.readFile(file);
    size_t sz = max(ori.length(), size + offset);
    BytePtr tmp = new Byte[sz];
    memset(tmp, 0, sz);
    memcpy(tmp, ori.d_ptr(), ori.length());
    memcpy(tmp + offset, buf, size);
    fs.writeFile(file, ByteArray(sz, tmp));
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
    } catch (const char* s) {
        return -1;
    }
    return 0;
}
static int fs_mkdir(const char* path, mode_t mod)
{
    cerr << endl
         << "call fs_mkdir" << endl;
    string name = "";
    inum_t fa = 0;
    auto file = parse(path, fa, name);
    if (name == "") {
        cerr << "No directory name detected.";
        return -1;
    }
    try {
        cerr << "creat directory '" << name << "' under " << file << endl;
        fs.createNode(file, name, 1);
    } catch (const char* s) {
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
    } catch (const char* s) {
        return -1;
    }
    return 0;
}
static int fs_rmdir(const char* path)
{
    cerr << endl
         << "call fs_rmdir" << endl;
    string s = "";
    inum_t father = 0;
    auto file = parse(path, father, s);
    try {
        fs.removeNode(father, file);
    } catch (const char* s) {
        cerr << s << endl;
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
    .write = fs_write,
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
