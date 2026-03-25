#include "include/graphics.h"
#include "include/vga.h"

static uint32_t framebuffer = (uint32_t)0xa0000;
static size_t SCREEN_W     = 640;
static size_t SCREEN_H     = 480;
static size_t SCREEN_AREA  = 640*480;
static uint16_t pitch = 0;
static uint8_t bpp = 0;
// static size_t SCREEN_AREA  = 320*300;

struct VesaModeInfo* _vesa_info_ptr;

static size_t cursorx = 0;
static size_t cursory = 0;

void init_graphics(struct VesaModeInfo* vesa_info_ptr)
{
    _vesa_info_ptr = vesa_info_ptr;
    framebuffer = vesa_info_ptr->physbase;

    SCREEN_W = vesa_info_ptr->Xres;
    SCREEN_H = vesa_info_ptr->Yres;
    SCREEN_AREA = SCREEN_W*SCREEN_H;

    pitch = vesa_info_ptr->pitch;
    bpp = vesa_info_ptr->bpp;
}

void init_graphics_addr(uint32_t vram_addr, struct VesaModeInfo* vesa_info_ptr)
{
    init_graphics(vesa_info_ptr);
    framebuffer = vram_addr;
}

void graphics_putpixel(uint32_t x, uint32_t y, uint32_t colour)
{
    *(uint32_t*)(framebuffer + y*pitch + x*bpp/8) = colour;
}

void graphics_fillrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t colour)
{
    for (uint32_t j = y; j < y+h; j++)
    {
        for (uint32_t i = x; i < x+w; i++)
        {
            graphics_putpixel(i, j, colour);
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
            *(uint8_t*)(framebuffer + SCREEN_W*(y+j) + x+i) = colour;
        }
    }
}

void graphics_draw_pixelmap(PixelMap map, uint32_t colour)
{
    for (int j = 0; j < PIXELMAP_SIZE; j++)
        for (int i = 0; i < PIXELMAP_SIZE; i++)
        {
            if (map.pixels[PIXELMAP_SIZE*j + i])
                graphics_putpixel(cursorx * PIXELMAP_SIZE + i, cursory * PIXELMAP_SIZE + j, colour);
        }
    cursorx++;
    if (cursorx*PIXELMAP_SIZE > pitch - PIXELMAP_SIZE)
    {
        cursory++;
        cursorx = 0;
    }
}

void graphics_clrscr()
{
    for (size_t i = 0; i < SCREEN_AREA; i++)
    {
        *(uint8_t*)(framebuffer + i) = 0;
    }
}

void graphics_clrscr_vga()
{
    for (size_t i = 0; i < SCREEN_AREA; i++)
    {
        *(uint8_t*)(framebuffer + i) = VGA_COLOUR_BLACK;
    }
}
