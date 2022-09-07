#ifndef CRC32_H
#define CRC32_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


unsigned long Crc32_ComputeBuf(unsigned long inCrc32, const void* buf, size_t bufLen);

#endif
