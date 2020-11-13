#include <bits/stdc++.h>
#include "FS.h"
#include "Interface.cpp"
int main(int argc, char *argv[]){
	FS demo;
	demo.fsInfo();
	return fuse_main(argc,argv,&fs_operations,NULL);
	return 0;
}
