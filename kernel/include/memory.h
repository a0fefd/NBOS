#ifndef _MEMORY_H
#define _MEMORY_H

#include "../../libc/include/stdint.h"
void* memcpy(void* dst, const void* src, size_t num);
void* memset(void* ptr, int value, uint16_t num);
int memcmp(const void* ptr1, const void* ptr2, uint16_t num);

#endif