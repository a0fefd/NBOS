#include "include/vga.h"

uint8_t vga_entry_colour(VGA_COLOUR foreground, VGA_COLOUR background)
{
    return foreground | background << 4;
}

uint16_t vga_entry(unsigned char uc, uint8_t colour)
{
    return (uint16_t) uc | (uint16_t) colour << 8;
}