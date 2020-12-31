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
    fs_mknod("/aa", 0, 0);
    char tmp[] = "hello";
    char ttmp[20];
    fs_write("/aa", tmp, 6, 0, nullptr);
    fs_read("/aa", ttmp, 20, 0, nullptr);
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
