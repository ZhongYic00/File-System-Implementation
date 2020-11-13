#include<bits/stdc++.h>
const long long GB=1<<29;
const long long DEVICESIZE=5*GB;
typedef char u8;
typedef char* ByteArray;
class HardwareAbstractionLayer{
public:
	enum ErrorInfo{
		LBAIllegal,
		LengthIllegal,
		Overwhelmed,
		Unknown,
	};
	HardwareAbstractionLayer(){
//		memset(blockDevice,0,sizeof(blockDevice));
	}
	int read(const int LBA, const size_t _length, const ByteArray _data) const;
	int write(const int LBA, const ByteArray _data, const size_t _length);
	size_t deviceSize() const;
private:
	static u8 blockDevice[DEVICESIZE];
	inline  static int _addressJudge(const int &LBA, const size_t &_length);
#ifndef RELEASE
	void _MemoryTest() const{
		printf("%lld\n",sizeof(blockDevice));
		srand(time(NULL));
		for(long long i=10000;i<=DEVICESIZE;i+=10000){
			printf("blockDevice[%d]: %d(addr=%p, d=%d)\n",i,blockDevice[i],&blockDevice[i],&blockDevice[i]-&blockDevice[i-10000]);
		}
		for(int i=100,j;i>=0;i--){
			printf("blockDevice[%d]: %d(addr=%p)\n",j=rand()%DEVICESIZE,blockDevice[j],&blockDevice[j]);
		}
	}
#endif
};
