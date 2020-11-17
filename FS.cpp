#include "include/FS.h"
void FS::fsInfo() const
{
    printf("FileSystem Size: %ull\n", HAL.deviceSize());
}
