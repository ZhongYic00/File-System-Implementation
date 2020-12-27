#ifndef ACCESS_H
#define ACCESS_H
#include <bits/stdc++.h>
using namespace std;

typedef unsigned char NodeType;
typedef unsigned char AccessGroup;
typedef unsigned char CheckCode;

class Access
{
public:
    Access();
    Access(NodeType t, AccessGroup o, AccessGroup r, AccessGroup oth);
    string info();
/*
    void chmod(string s);
*/
    void getHammingCode();
    bool checkLegitimacy();
    inline bool isDirectory() const;

private:
	CheckCode checkCode1 : 1;
	CheckCode checkCode2 : 1;
	NodeType type 		 : 1;
	CheckCode checkCode3 : 1;
	AccessGroup owner 	 : 3;
	CheckCode checkCode4 : 1;
	AccessGroup root 	 : 3;
	AccessGroup other 	 : 3;
};
#endif // ACCESS_H
