#ifndef ACCESS_H
#define ACCESS_H
#include <bits/stdc++.h>
using namespace std;

typedef unsigned char NodeType;
typedef unsigned char AccessGroup;

class Access
{
public:
    Access();
    Access(NodeType t, AccessGroup o, AccessGroup r, AccessGroup oth);
    string info();

private:
	NodeType type : 1;
	AccessGroup owner : 3;
	AccessGroup root : 3;
	AccessGroup others : 3;
};

#endif // ACCESS_H
