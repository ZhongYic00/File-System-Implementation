#include "extenttree.h"
const int SumBlo = 1 << 30;
vector <int>q;
void pushup(node* p)
{
    if (p->l != NULL && p->r != NULL) {
        p->size = p->l->size + p->r->size;
        p->maxleft = max(p->l->maxleft, p->r->maxleft);
        p->flag = p->l->flag | p->r->flag;
        p->left = p->r->left + p->l->left;
        return;
    }
    if (p->r == NULL) {
        p->maxleft = max(p->l->maxleft, p->maxleft >> 1);
        p->size = p->l->size;
        p->flag = p->l->flag | p->r->flag;
        p->left = p->l->left + p; 
    }
}
void build(node* p, const int d, LBA_t& dd, const int now, const int ref)
{
    if (dd == 0)return;
    if (!p->bl)
    {
        p->size = 1; p->bl = d; p->left = d; p->maxleft = d; p->logbl = log(d) / log(2);
        numt++;
    }
    if (p->bl == dd && p->left == dd)
    {
        p->num = now; dd = 0; p->left = 0; p->maxleft = 0;
        p->flag = 1 << now;
        p->ref = ref;
        return;
    }
    else if (dd < 2 * p->bl)
    {
        dd -= p->bl;
        p->num = now; p->left = 0; p->maxleft = 0;
        p->ref = ref;
        p->flag = 1 << now;
        return;
    }
    if (p->maxleft >= dd)
    {
        if (p->l == null) { p->l = new node; build(p->l, d >> 1, dd, now); }
        if (p->r == null) { p->r = new node; build(p->r, d >> 1, dd, now); }
    }
    pushup(p);// check(p);
}
void Release(node* p, int now, int d)
{
    if (p->num == now && p->bl == d)
    {
        p->flag = 0; p->left = p->bl; p->maxleft = p->bl; p->size = 0;
        check(p);
        return;
    }
    if ((p->l->flag >> (now - 1)) & 1)Release(p->l, now);
    pushup(p); //check(p);
    if ((p->r->flag >> (now - 1)) & 1)Release(p->r, now);
    pushup(p); //check(p);
}
void dfs1(node* p,int &num,node *a)
{
    if (p->l)dfs(p->l,num,a);
    if (p->r)dfs(p->r,num,a);
    p = a + num; num++;
}
ExtentTree::ExtentTree(const ByteArray& d)
{
    node (*a)= new node [d.length()];
    memcpy(a, d, d.length());
    root = a+0;
    dfs1(root,0,a);
    delete a;
    a = NULL;
}
void dfs2(node *p,int &num,node *a)
{
    if(p->l)dfs2(p -> l, num, a);
    if(p->r)dfs2(p -> r, num, a);
    num++;
    a+num = p;
    delete p;
    p = NULL;
}
ByteArray ExtentTree::dataExport()const
{
    node (*a)=new node [numt];
    dfs2(root, 0, a);
    ByteArray x(sizeof(a), a);
    delete a;
    a = NULL;
    return x;
}
list<pair<LBA_t, LBA_t>> ExtentTree::allocateExtents(LBA_t blks)
{
    cnt++;
    build(root, SumBlo, blks, cnt,1);
    sort(q, q + q.size - 1);
    list<pair< LBA_t, LBA_t>>qq;
    for (int i = 0; i < q.size(); i++)qq.push_back(make_pair<cnt, q[i]>);
    return qq;
}
list<pair<LBA_t, LBA_t>> ExtentTree::allocateExtent(int references)
{
    cnt++;
    build(root, SumBlo, 2, cnt, references);
    sort(q, q + q.size - 1);
    list<pair< LBA_t, LBA_t>>qq;
    for (int i = 0; i < q.size(); i++)qq.push_back(make_pair<cnt, q[i]>);
    return qq;
}
void ExtentTree::releaseExtent(LBA_t startpos, LBA_t length)
{
    Release(root, startpos, length);
}
inline void ExtentTree::releaseExtent(list<pair<LBA_t, LBA_t>> extents)
{
    while (!extents.empty())
    {
        int a, b;
        a = extents.front().first;
        b = extents.front().second;
        Release(root, a, b);
        extents.pop_front();
    }
}
#endif
