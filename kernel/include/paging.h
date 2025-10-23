#pragma once

#include "../../libc/include/stdint.h"

void enable_full_paging(uint32_t phys, uint32_t size, void (*__func_printf)(const char* str, ...));