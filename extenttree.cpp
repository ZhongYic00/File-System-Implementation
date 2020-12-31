#include "extenttree.h"
void ExtentTree::pushup(node* p)
{
    if (p->lson != nullptr && p->rson != nullptr) {
        p->maxleft = max(p->lson->maxleft, p->rson->maxleft);
        p->flag = p->lson->flag | p->rson->flag;
        p->left = p->lson->left + p->rson->left;
        return;
    } else if (p->rson == nullptr && p->lson != nullptr) {
        p->maxleft = p->bl >> 1;
        p->left = p->lson->left + p->bl >> 1;
        p->flag = 0;

        return;
    } else if (p->rson != nullptr && p->lson == nullptr) {
        p->flag = 0;
        p->maxleft = p->bl >> 1;
        p->left = p->bl >> 1 + p->rson->left;
        return;
    } else if (p->rson == nullptr && p->lson == nullptr) {
        delete p;
        p = nullptr;
        return;
    }
}
int ExtentTree::update(node* p, int l, int r, int ref, LBA_t blk, LBA_t target)
{
    if (p->bl == target && p->maxleft == target) {
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
        p->lson = new node();
        p->lson->bl = blk >> 1, p->lson->l = l, p->lson->r = mid;
        p->lson->left = blk >> 1;
        p->lson->maxleft = blk >> 1;
    }
    if (p->lson->maxleft >= target) {
        int x = update(p->lson, l, mid, ref, blk >> 1, target);
        pushup(p);
        return x;
    }
    if (p->rson == nullptr) {
        p->rson = new node();
        p->rson->bl = blk >> 1, p->rson->l = mid + 1, p->lson->left = blk >> 1;
        p->rson->r = r;
        p->rson->maxleft = blk >> 1;
    }
    if (p->rson->maxleft >= target) {
        int x = update(p->rson, mid + 1, r, ref, blk >> 1, target);
        pushup(p);
        return x;
    }
}
void ExtentTree::Release(node* p, int now, LBA_t d)
{
    if (p->l > now || p->bl + p->l < now)
        return;
    if (p->lson) {
        if (p->lson->l == now && p->lson->bl == d) {
            p->lson->ref--;
            if (p->lson->ref == 0) {
                delete p->lson;
                p->lson = nullptr;
                NumOfUpd--;
                pushup(p);
                return;
            }
        }
    }
    if (p->rson) {
        if (p->rson->l == now && p->rson->bl == d) {
            p->rson->ref--;
            if (p->rson->ref == 0) {
                delete p->rson;
                p->rson = nullptr;
                NumOfUpd--;
                pushup(p);
                return;
            }
        }
    }
    int mid = (p->l + p->r) / 2;
    if (now > mid)
        Release(p->rson, now, d);
    else
        Release(p->lson, now, d);
    pushup(p);
}
void ExtentTree::setRoot(int x, int sumblo, int l)
{
    root[x] = new node();
    (root[x])->l = l;
    (root[x])->r = l + sumblo - 1;
    (root[x])->bl = sumblo;
    (root[x])->left = sumblo;
    (root[x])->maxleft = sumblo;
    (root[x])->ref = 0;
}
void ExtentTree::build(node* p, int l, int r, int ref, LBA_t blk, LBA_t target, int pos)
{
    if (p->l == pos && blk == target) {
        p->left = 0;
        p->ref = ref;
        p->maxleft = 0;
        NumOfUpd++;
        return;
    }
    int mid = (l + r) / 2;
    if (pos <= mid) {
        if (p->lson == nullptr) //build new node on tree
        {
            p->lson = new node();
            p->lson->bl = blk >> 1, p->lson->l = l, p->lson->left = blk >> 1;
            p->lson->r = mid;
            p->lson->maxleft = blk >> 1;
        }
        build(p->lson, l, mid, ref, blk >> 1, target, pos);
        pushup(p);
        return;
    }
    if (pos > mid) {
        if (p->rson == nullptr) //build new node on tree
        {
            p->rson = new node();
            p->rson->bl = blk >> 1, p->rson->l = mid + 1, p->rson->left = blk >> 1;
            p->rson->r = r;
            p->rson->maxleft = blk >> 1;
        }
        build(p->rson, mid + 1, r, ref, blk >> 1, target, pos);
        pushup(p);
        return;
    }
    pushup(p);
}
list<pair<LBA_t, LBA_t>> ExtentTree::allocateExtents(LBA_t blks)
{
    cerr << "call allocateExtents" << blks << endl;
    LBA_t no = blks;
    int cnt = 0, dat = log(blks) / log(2);
    list<pair<LBA_t, LBA_t>> X;
    bool flag = false;
    while (no != 0) {
        for (int i = 0; i < ArrayNum; i++) {
            int now = 1;
            if (root[i]->maxleft >= (1 << dat)) {
                X.push_back(make_pair(update(root[i], now, now + SumBloArray[i] - 1, 1, SumBloArray[i], 1ll << dat), dat));
                no = no - (1ll << dat);
                now += SumBloArray[i];
                dat = log(no) / log(2);
                flag = true;
                break;
            }
            now += SumBloArray[i];
        }
        if (flag)
            continue;
        dat--;
    }
    for (auto i : X) {
        cerr << i.first << ',' << i.second << ' ';
    }
    return X;
}
list<pair<LBA_t, LBA_t>> ExtentTree::allocateExtent(int references)
{
    cerr << "call allocateExtent " << references << endl;
    list<pair<LBA_t, LBA_t>> X;
    int now = 1;
    for (int i = 0; i < ArrayNum; i++) {
        if (root[i]->maxleft >= 1) {
            X.push_back(make_pair(update(root[i], now, now - 1 + (SumBloArray[i]), references, SumBloArray[i], 1ll), 0));
            break;
        }
        now += SumBloArray[i];
    }
    for (auto i : X) {
        cerr << i.first << ',' << i.second << ' ';
    }
    cerr << endl;
    return X;
}
void ExtentTree::releaseExtent(LBA_t startpos, LBA_t length)
{
    cerr << "call releaseExtent " << startpos << ' ' << length << endl;
    int now = 1;
    length = pow(2, length);
    for (int i = 0; i < ArrayNum; i++) {
        if (now + (SumBloArray[i] - 1 >= startpos)) {
            Release(root[i], startpos, length);
            break;
        }
    }
}
void ExtentTree::build(int lenth, int pos, int ref)
{
    int now = 1;
    for (int i = 0; i < ArrayNum; i++) {
        if (now + (SumBloArray[i]) > pos) {
            build(root[i], now, now + (SumBloArray[i]) - 1, ref, (SumBloArray[i]), lenth, pos);
            return;
        }
    }
}
ExtentTree::ExtentTree(const ByteArray& d)
    : NumOfUpd(0)
    , ArrayNum(0)
{
    int sum = SumBlo, l = 1;
    while (sum) {
        int dat = log(sum) / log(2);
        sum -= 1 << dat;
        ArrayNum++;
    }
    sum = SumBlo;
    root = new node*[ArrayNum];
    SumBloArray = new LBA_t[ArrayNum];
    ArrayNum = 0;
    while (sum) {
        int dat = log(sum) / log(2);
        sum = sum - (1 << dat);
        SumBloArray[ArrayNum] = 1 << dat;
        ArrayNum++;
    }
    for (int i = 0; i < ArrayNum; i++) { //cout<<"for��"<<SumBloArray[i]<<endl;
        setRoot(i, SumBloArray[i], l);
        l += (SumBloArray[i]);
    }
    date(*a) = new date[d.length() / sizeof(date)];
    memcpy(a, d.d_ptr(), d.length());
    for (int i = 0; i <= d.length() / sizeof(date); i++) {
        if ((a + i)->ref) {
            build((a + i)->lenth, (a + i)->startpos, (a + i)->ref);
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
    for (int i = 0; i < ArrayNum; i++)
        dateExportOnTree(root[i], Save, q);
    BytePtr tmp = new Byte[SizeForSave];
    memcpy(tmp, Save, SizeForSave);
    ByteArray rt(SizeForSave, tmp);
    delete[] tmp;
    tmp = NULL;
    delete[] Save;
    Save = nullptr;
    delete[] root;
    delete[] SumBloArray;
    return rt;
}
