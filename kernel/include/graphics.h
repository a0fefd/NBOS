#pragma once

#include "vga.h"
#include "kernel.h"

#define GRAPHICS_MEMORY 0xA0000

void graphics_fillrect(size_t x, size_t y, size_t w, size_t h, uint32_t colour);
// void graphics_fillrect_vga(size_t x, size_t y, size_t w, size_t h, VGA_COLOUR colour);
void graphics_fillrect_vga(size_t x, size_t y, size_t w, size_t h, uint8_t colour);

void graphics_clrscr();