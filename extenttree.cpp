#include "extenttree.h"
//bool ExtentTree::setroot = false;
void ExtentTree::pushup(node* p)
{
    if (p->lson != nullptr && p->rson != nullptr) {
        //p->maxleft = max(p->lson->maxleft, p->rson->maxleft);
        p->flag = p->lson->flag | p->rson->flag;
        p->left = p->lson->left + p->rson->left;
        return;
    } else if (p->rson == nullptr && p->lson != nullptr) { //from this
        //p->maxleft = max(p->lson->maxleft, p->maxleft >> 1);
        p->left = p->lson->left + p->maxleft >> 1;
        p->flag = 0;
        for (int i = 0; i < log(p->bl) / log(2); i++)
            p->flag |= 1 << i;
        return;
    } else if (p->rson != nullptr && p->lson == nullptr) {
        p->flag = 0;
        for (int i = 0; i < log(p->bl) / log(2); i++)
            p->flag |= 1 << i;
        return;
    } //to this are useless
    else if (p->rson == nullptr && p->lson == nullptr) {
        delete p;
        p = nullptr;
        return;
    }
}
int ExtentTree::update(node* p, int l, int r, int ref, LBA_t blk, LBA_t target)
{
    if (p->bl == target && p->left == target) {
        p->left = 0;
        p->ref = ref;
        p->maxleft = 0;
        p->flag = 0;
        NumOfUpd++;
        return p->l;
    }
    int mid = (l + r) / 2;
    if (p->lson == nullptr) //build new node on tree
    {
        p->lson->bl = blk, p->lson->l = l, p->left = blk;
        p->maxleft = blk;
        for (int i = 0; i < log(blk) / log(2); i++)
            p->flag |= 1 << i;
    }
    if ((p->lson->flag >> int((log(target) / log(2) - 1))) & 1) {
        int x = update(p->lson, l, mid, ref, blk >> 1, target);
        pushup(p);
        return x;
    }
    if (p->rson == nullptr) //build new node on tree
    {
        p->rson->bl = blk, p->rson->l = mid + 1, p->left = blk;
        p->maxleft = blk;
        for (int i = 0; i < log(blk) / log(2); i++)
            p->flag |= 1 << i;
    }
    if ((p->rson->flag >> int((log(target) / log(2) - 1))) & 1) {
        int x = update(p->rson, mid + 1, r, ref, blk >> 1, target);
        pushup(p);
        return x;
    }
}
void ExtentTree::Release(node* p, int now, LBA_t d)
{
    if (p->l == now && p->bl == d) {
        (p->ref)--;
        if (p->ref == 0) {
            p->left = p->bl;
            for (int i = 0; i < log(p->bl) / log(2); i++)
                p->flag |= 1 << i;
            delete p;
            p = nullptr;
            NumOfUpd--;
            return;
        }
    }
    int mid = (p->l + p->r) / 2;
    if (now > mid)
        Release(p->rson, now, d);
    else
        Release(p->lson, now, d);
    pushup(p);
}
void ExtentTree::setRoot()
{
    if (setroot)
        return;
    for (int i = 0; i <= 30; i++)
        root->flag |= 1 << i;
    root->l = 1;
    root->r = SumBlo;
    root->bl = SumBlo;
    root->left = SumBlo;
    root->ref = 0;
    setroot = true;
}
void ExtentTree::build(node* p, int l, int r, int ref, LBA_t blk, LBA_t target, int pos)
{
    if (p->l == pos && blk == target) {
        p->ref = ref;
        return;
    }
    int mid = (l + r) / 2;
    if (p->lson == nullptr) //build new node on tree
    {
        p->lson->bl = blk, p->lson->l = l, p->left = blk;
        p->maxleft = blk;
        for (int i = 0; i < log(blk) / log(2); i++)
            p->flag |= 1 << i;
    }
    if (pos <= mid)
        build(p->lson, l, mid, ref, blk >> 1, target, pos);
    if (p->rson == nullptr) //build new node on tree
    {
        p->rson->bl = blk, p->rson->l = mid + 1, p->left = blk;
        p->maxleft = blk;
        for (int i = 0; i < log(blk) / log(2); i++)
            p->flag |= 1 << i;
    }
    if (pos > mid)
        build(p->rson, mid + 1, r, ref, blk >> 1, target, pos);
    pushup(p);
}
ExtentTree::ExtentTree(const ByteArray& d)
    : root(new node)
    , setroot(false)
    , NumOfUpd(0)
{
    date(*a) = new date[d.length() / sizeof(date)];
    memcpy(a, d.d_ptr(), d.length());
    setRoot();
    for (int i = 0; i <= d.length() / sizeof(date); i++) {
        if ((a + i)->ref) {
            build(root, 1, SumBlo, (a + i)->ref, SumBlo, (a + i)->lenth, (a + i)->startpos);
        }
    }
    delete[] a;
    a = nullptr;
}
void ExtentTree::dateExportOnTree(node* p, date* _this, int& cnt) const
{
    if (p->ref) {
        (_this + cnt)->startpos = p->l;
        (_this + cnt)->ref = p->ref;
        (_this + cnt)->lenth = p->bl;
        cnt++;
        return;
    }
    if (p->lson)
        dateExportOnTree(p->lson, _this, cnt);
    if (p->rson)
        dateExportOnTree(p->rson, _this, cnt);
    delete p;
    p = nullptr;
}
ByteArray ExtentTree::dataExport() const
{
    date* Save = new date[NumOfUpd];
    int q = 0, SizeForSave = NumOfUpd * sizeof(date);
    dateExportOnTree(root, Save, q);
    BytePtr tmp = new Byte[SizeForSave];
    memcpy(tmp, Save, SizeForSave);
    ByteArray rt(SizeForSave, tmp);
    delete[] tmp;
    tmp = nullptr;
    delete[] Save;
    Save = nullptr;
    return rt;
}
list<pair<LBA_t, LBA_t>> ExtentTree::allocateExtents(LBA_t blks)
{
    LBA_t now = blks;
    int cnt = 0, dat = log(blks) / log(2);
    list<pair<LBA_t, LBA_t>> X;
    while (now) {
        if ((root->flag >> dat) & 1) {
            X.push_back(make_pair(update(root, 1, SumBlo, 1, SumBlo, 1 << dat), dat));
            now -= 1 << dat;
            dat--;
            continue;
        }
        dat--;
    }
    return X;
}
list<pair<LBA_t, LBA_t>> ExtentTree::allocateExtent(int references)
{
    list<pair<LBA_t, LBA_t>> X;
    X.push_back(make_pair(update(root, 1, SumBlo, references, SumBlo, 1), 0));
    return X;
}
void ExtentTree::releaseExtent(LBA_t startpos, LBA_t length)
{
    Release(root, startpos, length);
}
