#pragma once

#include "vga.h"
#include "kernel.h"

#define PIXELMAP_SIZE 8

typedef struct pixelmap_st {
    uint8_t pixels[PIXELMAP_SIZE*PIXELMAP_SIZE];
} PixelMap;

extern PixelMap kbd_pixelmaps[256];

uint32_t get_fg_colour();
void set_fg_colour(uint32_t colour);

void init_graphics(volatile struct VesaModeInfo* vesa_info_ptr);
void init_graphics_addr(uint32_t vram_addr, volatile struct VesaModeInfo* vesa_info_ptr);

void graphics_putpixel(uint32_t x, uint32_t y, uint32_t colour);
uint32_t graphics_getpixel(uint32_t x, uint32_t y);

void graphics_fillrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t colour);
// void graphics_fillrect_vga(size_t x, size_t y, size_t w, size_t h, VGA_COLOUR colour);
void graphics_fillrect_vga(size_t x, size_t y, size_t w, size_t h, uint8_t colour);

void graphics_draw_pixelmap(PixelMap map, uint32_t colour);
void graphics_backspace_pixelmap();
void graphics_return_pixelmap();

void graphics_clrscr();
void graphics_clrscr_vga();
