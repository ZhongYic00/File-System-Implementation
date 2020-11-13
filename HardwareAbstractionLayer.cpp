#include "HardwareAbstractionLayer.h"
u8 HardwareAbstractionLayer::blockDevice[DEVICESIZE];
inline int HardwareAbstractionLayer::_addressJudge(const int &LBA, const size_t &_length){
	//need optimize
	if(LBA<0 || LBA>=DEVICESIZE) return LBAIllegal;
	else if(_length<=0)return LengthIllegal;
	else if(LBA+_length>=DEVICESIZE) return Overwhelmed;
	else return 0;
}
int HardwareAbstractionLayer::read(const int LBA, const size_t _length, const ByteArray _data) const{
	if(!_addressJudge(LBA,_length)) return _addressJudge(LBA,_length);
	else{
		memcpy(_data,(u8*)blockDevice+LBA,_length);	//memcpy(dst,src,size)
		return 0;
	}
	return Unknown;
}
int HardwareAbstractionLayer::write(const int LBA, const ByteArray _data, const size_t _length){
	if(!_addressJudge(LBA,_length)) return _addressJudge(LBA,_length);
	else{
		memcpy((u8*)blockDevice+LBA, _data, _length);
		return 0;
	}
	return Unknown;
}
