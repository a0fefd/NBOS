#ifndef _TUTILS_H
#define _TUTILS_H

#include "vga.h"
#include "../../libc/include/string.h"

extern size_t terminal_row;
extern size_t terminal_column;
extern uint8_t terminal_color;
extern uint16_t* terminal_buffer;

void terminal_init(VGA_COLOUR foreground, VGA_COLOUR background);
void terminal_setcolour(uint8_t colour);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_write_left_anchor(const char* buf, size_t x, size_t y);
void terminal_write_right_anchor(const char* buf, size_t x, size_t y);

#endif