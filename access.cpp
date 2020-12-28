#include "include/access.h"
#include <bits/stdc++.h>
using namespace std;

Access::Access()
{
    type = 0;
    owner = 0;
    root = 0;
    others = 0;
}
Access::Access(NodeType t, AccessGroup o, AccessGroup r, AccessGroup oth)
{
    type = t;
    owner = o;
    root = r;
    others = oth;
}
//Initialization

string Access::info()
{
    auto accessDecode = [](int o) -> string {
        return string("") + (o & (1 << 2) ? 'r' : '-')
            + (o & (1 << 1) ? 'w' : '-')
            + (o & 1 ? 'x' : '-');
    };
    return string(type ? "d" : "-")
        + accessDecode(owner)
        + accessDecode(root)
        + accessDecode(others);
}
//Get information of a file/directory.

void Access::getHammingCode()
{
    checkCode1 = ((type & 1) + (owner >> 2 & 1) + (owner & 1) + (root >> 2 & 1) + (root & 1) + (other >> 1 & 1)) % 2;
    checkCode2 = ((type & 1) + (owner >> 1 & 1) + (owner & 1) + (root >> 1 & 1) + (root & 1) + (other & 1)) % 2;
    checkCode3 = ((owner >> 2 & 1) + (owner >> 1 & 1) + (owner & 1) + (other >> 2 & 1) + (owner >> 1 & 1) + (other & 1)) % 2;
    checkCode4 = ((root >> 2 & 1) + (root >> 1 & 1) + (root & 1) + (other >> 2 & 1) + (owner >> 1 & 1) + (other & 1)) % 2;
}
//To get HammingCode.

bool Access::isValid()
{
    return !(((checkCode1 & 1) + (checkCode2 & 1) + (type & 1) + (checkCode3 & 1) + (owner >> 2 & 1) + (owner >> 1 & 1) + (owner & 1) + (root >> 2 & 1) + (root >> 1 & 1) + (root & 1) + (other >> 2 & 1) + (owner >> 1 & 1) + (other & 1)) % 2);
}
//Check whether a node is valid or not.

bool Access::isDirectory()
{
    return type;
}
//To judge whether this node is a directory.

int chmod(string s)
{
	if (!isValid()) return 1;
    owner = s[0] - '0';
    root = s[1] - '0';
    other = s[2] - '0';
    getHammingCode();
    return 0;
}
//Change mod.
