#include "include/graphics.h"

uint8_t* vram = (uint8_t*)GRAPHICS_MEMORY;

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