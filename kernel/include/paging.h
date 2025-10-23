#pragma once

#include "memory.h"
// #include "../../libc/include/stdint.h" // in memory.h



void enable_paging(uint32_t phys, uint32_t virt, uint32_t size);
void paging_debug(void (*__func_printf)(const char* str, ...));