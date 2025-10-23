#include "include/graphics.h"

static uint8_t* vram = (uint8_t*)0xa0000;
static size_t SCREEN_W     = 320;
static size_t SCREEN_H     = 300;
static size_t SCREEN_AREA  = 320*300;

void init_graphics(uint8_t* vram_addr, struct VesaModeInfo* vesa_info_ptr)
{
    vram = vram_addr;

    SCREEN_W = vesa_info_ptr->Xres;
    SCREEN_H = vesa_info_ptr->Yres;
    SCREEN_AREA = SCREEN_W*SCREEN_H;
}

void graphics_fillrect(size_t x, size_t y, size_t w, size_t h, uint32_t colour)
{
    for (size_t j = 0; j < h; j++)
    {
        for (size_t i = 0; i < w; i++)
        {
            size_t offs = 3*( SCREEN_W*(y+j) + x+i );
            vram[offs    ] = (colour      ) & 0xff;
            vram[offs + 1] = (colour >>  8) & 0xff;
            vram[offs + 2] = (colour >> 16) & 0xff;
        }
    }

}
// void graphics_fillrect_vga(size_t x, size_t y, size_t w, size_t h, VGA_COLOUR colour)
void graphics_fillrect_vga(size_t x, size_t y, size_t w, size_t h, uint8_t colour)
{
    for (size_t j = 0; j < h; j++)
    {
        for (size_t i = 0; i < w; i++)
        {
            *(vram + SCREEN_W*(y+j) + x+i) = colour;
        }
    }
}

void graphics_clrscr()
{
    for (size_t i = 0; i < SCREEN_AREA; i++)
    {
        *(vram + i) = VGA_COLOUR_BLACK;
    }
}