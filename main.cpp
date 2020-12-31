#include "include/FS.h"
#include <bits/stdc++.h>
#ifdef linux
#include "Interface.cpp"
#endif

#define fuse_test
//#define dbg
//#define unit
int main(int argc, char* argv[])
{
#ifdef unit
    fs_init(nullptr, nullptr);
    char buf[] = "aa";
    fs_mknod("/1", 0, 0);
    fs_mknod("/2", 0, 0);
    fs_mknod("/3", 0, 0);
    fs_mknod("/4", 0, 0);
    fs_mknod("/5", 0, 0);
    fs_mknod("/6", 0, 0);
    fs_mknod("/7", 0, 0);
    fs_mknod("/8", 0, 0);
    fs_mknod("/9", 0, 0);
    fs_mknod("/10", 0, 0);
    fs_mknod("/11", 0, 0);
    fs_write("/1", buf, 3, 0, nullptr);
    fs_write("/2", buf, 3, 0, nullptr);
    fs_write("/3", buf, 3, 0, nullptr);
    fs_write("/4", buf, 3, 0, nullptr);
    fs_write("/5", buf, 3, 0, nullptr);
    fs_write("/6", buf, 3, 0, nullptr);
    fs_write("/7", buf, 3, 0, nullptr);
    fs_write("/8", buf, 3, 0, nullptr);
    fs_write("/9", buf, 3, 0, nullptr);
    fs_write("/10", buf, 3, 0, nullptr);
    fs_write("/11", buf, 3, 0, nullptr);
    fs_destroy(nullptr);
#endif
#ifdef dbg
    FS fs;
    fs.fsInit();
    fs.test();
    //fs.fsExit();
#endif
#ifdef linux
#ifdef fuse_test
    return fuse_main(argc, argv, &fs_operations, NULL);
#endif
#else
    return 0;
#endif
}
