#include <bits/stdc++.h>
#include "FS.h"
#include "Interface.cpp"
int main(int argc, char *argv[]){
	FS demo;
	demo.fsInfo();
#ifdef linux
	return fuse_main(argc,argv,&fs_operations,NULL);
#elif
	return 0;
#endif
}
