#include "include/FS.h"
#include <bits/stdc++.h>
#ifdef linux
#include "Interface.cpp"
#endif
int main(int argc, char* argv[])
{
    FS fs;
    fs.fsInit();
    fs.fsExit();
#ifdef linux
    //return fuse_main(argc, argv, &fs_operations, NULL);
#else
    return 0;
#endif
}
