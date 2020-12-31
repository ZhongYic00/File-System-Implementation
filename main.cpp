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
    auto node = new DirectoryNode(FSNode(), ByteArray());
    node->addSubnode(1, "a");
    node->updateDataExtentLBA(2);
    node->print();
    auto nodetmp = node->nodeDataExport();
    auto dirtmp = node->dataExport();
    auto node1 = new DirectoryNode(FSNode(nodetmp), dirtmp);
    node1->print();
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
    /*inum_t fa;
    string name;
    cerr << parse("/", fa, name);*/
#else
    return 0;
#endif
}
