#include <bits/stdc++.h>
#include "FS.h"
#ifdef linux
#include "Interface.cpp"
#endif
int main(int argc, char *argv[]){
	FS demo;
	demo.fsInfo();
#ifdef linux
	return fuse_main(argc,argv,&fs_operations,NULL);
#else
	return 0;
#endif
}
