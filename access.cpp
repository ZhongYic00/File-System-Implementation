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

int main()
{
    /*
	cout << sizeof(Access) << endl;
	Access a(0, 7, 6, 5);
	Access b(1, 4, 3, 2);
	Access c(0, 1, 1, 1);
	cout << a.info() << endl;
	cout << b.info() << endl;
	cout << c.info() << endl;
	*/
    return 0;
}
