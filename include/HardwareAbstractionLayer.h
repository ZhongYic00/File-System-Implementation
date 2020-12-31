#ifndef HARDWAREABSTRACTIONLAYER_H
#define HARDWAREABSTRACTIONLAYER_H
#include "constants.h"
#include <bits/stdc++.h>
const long long DEVICESIZE = DEVICE_SIZE;
class HardwareAbstractionLayer {
public:
    enum ErrorInfo {
        LBAIllegal,
        LengthIllegal,
        Overwhelmed,
        Unknown,
    };
    HardwareAbstractionLayer()
    {
        std::ifstream i("disk_save.dim", std::ios::binary);
        //i.read(reinterpret_cast<char*>(blockDevice), DEVICESIZE);
    }
    ~HardwareAbstractionLayer();
    int read(const LBA_t LBA, const LBA_t _length, const BytePtr _data) const; //read data at LBA of length _length(measured by block), write result to _data(size of _data must be more than _length)
    int write(const LBA_t LBA, const BytePtr _data, const LBA_t _length); //write data to LBA of size _length(measured by block), data is passed by _data
    inline size_t deviceSize() const;

private:
    static u8 blockDevice[DEVICESIZE];
    inline static int _addressJudge(const LBA_t& LBA, const LBA_t& _length);
#ifndef RELEASE
    void _MemoryTest() const
    {
        printf("%lld\n", sizeof(blockDevice));
        srand(time(NULL));
        for (long long i = 10000; i <= DEVICESIZE; i += 10000) {
            printf("blockDevice[%d]: %d(addr=%p, d=%d)\n", i, blockDevice[i], &blockDevice[i], &blockDevice[i] - &blockDevice[i - 10000]);
        }
        for (int i = 100, j; i >= 0; i--) {
            printf("blockDevice[%d]: %d(addr=%p)\n", j = rand() % DEVICESIZE, blockDevice[j], &blockDevice[j]);
        }
    }
#endif
};
inline size_t HardwareAbstractionLayer::deviceSize() const
{
    return sizeof(blockDevice);
}

#endif
