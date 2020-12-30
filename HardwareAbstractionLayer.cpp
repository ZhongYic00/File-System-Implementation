#include "include/HardwareAbstractionLayer.h"
u8 HardwareAbstractionLayer::blockDevice[DEVICESIZE];
inline int HardwareAbstractionLayer::_addressJudge(const LBA_t& LBA, const LBA_t& _length)
{
    // need optimize, bug: unsigned num & smaller than 0
    if (LBA >= DEVICESIZE)
        return LBAIllegal;
    else if (_length <= 0)
        return LengthIllegal;
    else if (LBA + _length >= DEVICESIZE)
        return Overwhelmed;
    else
        return 0;
}
int HardwareAbstractionLayer::read(const LBA_t LBA, const LBA_t _length, const BytePtr _data) const
{
    if (!_addressJudge(LBA, _length))
        return _addressJudge(LBA, _length);
    else {
        memcpy(_data, (u8*)blockDevice + LBA * BLOCKSIZE_BYTE, _length * BLOCKSIZE_BYTE); // memcpy(dst,src,size)
        return 0;
    }
    return Unknown;
}
int HardwareAbstractionLayer::write(const LBA_t LBA, const BytePtr _data, const LBA_t _length)
{
    if (!_addressJudge(LBA, _length))
        return _addressJudge(LBA, _length);
    else {
        memcpy((u8*)blockDevice + LBA * BLOCKSIZE_BYTE, _data, _length * BLOCKSIZE_BYTE);
        return 0;
    }
    return Unknown;
}
HardwareAbstractionLayer::~HardwareAbstractionLayer()
{
    std::ofstream o("disk_save.dim", std::ios::binary);
    o.write(reinterpret_cast<char*>(blockDevice), DEVICESIZE);
    o.close();
}
