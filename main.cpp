#include "include/FS.h"
#include <bits/stdc++.h>
#ifdef linux
#include "Interface.cpp"
#endif

//#define fuse_test
#define dbg
int main(int argc, char* argv[])
{
#ifdef dbg
    //FS fs;
    //fs.fsInit();
    fs_init(nullptr, nullptr);
    struct stat* buf = new struct stat;
    fs_getattr("/BDM", buf, nullptr);
    delete buf;
    //fs.test();
    //fs.fsExit();
#endif
#ifdef linux
#ifdef fuse_test
    return fuse_main(argc, argv, &fs_operations, NULL);
#endif
    /*inum_t fa;
    string name;
    cerr << parse("/", fa, name);*/
#else
    return 0;
#endif
}
