#ifndef EXTENTTREE_H
#define EXTENTTREE_H
#include "constants.h"
#include "fsnode.h"
#include <bits/stdc++.h>
const int LogSumBlo = 30;
const int SumBlo = 1 << LogSumBlo;
class ExtentTree {
public:
    struct node
    {
        int bl, logbl, maxleft, ref, left, l, r;//bl: size of extent
        LBA_t flag;
        node* lson, * rson;
        node()
        {
            flag = bl = logbl = left = maxleft = ref = l = r = 0;
            lson = rson = NULL;
        }
    };
    struct date
    {
        int startpos, lenth, ref;
    };
    ExtentTree(const ByteArray& d);
    ByteArray dataExport() const;
    list<pair<LBA_t, LBA_t>> allocateExtents(LBA_t blks);//new (extents)
    list<pair<LBA_t, LBA_t>> allocateExtent(int references);//new (extents)
    void releaseExtent(LBA_t startpos, LBA_t length);
    inline void releaseExtent(list<pair<LBA_t, LBA_t>> extents);
    void Release(node* p, int startpos, LBA_t length);//releaseextent
    void setRoot();
    void pushup(node* p);                                              //opreation on tree
    int update(node* p, int l, int r, int ref, LBA_t blk, LBA_t target);//
    void build(node* p,int l,int r,int ref,LBA_t blk,LBA_t target,int pos);//
    void dateExportOnTree(node* p,date *_this,int &cnt)const;
private:
    static node* root;
    static bool setroot;
    static int NumOfUpd;

    //ByteArray dataExport();
    //allocate extents by number of blocks, return extents sorted by ascending order(first:LBA,second:extentsize)
};
#endif