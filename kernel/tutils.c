#include "include/tutils.h"
#include "i686/include/io.h"
#include "include/vga.h"

size_t terminal_row;
size_t terminal_column;

VGA_COLOUR terminal_foreground_colour;
VGA_COLOUR terminal_background_colour;
uint8_t terminal_colour;

uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;
uint8_t* video_buffer = (uint8_t*)0xA0000;

void terminal_init(VGA_COLOUR foreground, VGA_COLOUR background)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_foreground_colour = foreground;
    terminal_background_colour = background;
    terminal_colour = vga_entry_colour(terminal_foreground_colour, terminal_background_colour);

    for (size_t y = 0; y < VGA_HEIGHT; y++) 
    {
        for (size_t x = 0; x < VGA_WIDTH; x++) 
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_colour);
        }
    }
}

void terminal_scrollback(int lines);

void terminal_setcolour(uint8_t color)
{
    terminal_colour = color;
}

int terminal_setcursor(size_t x, size_t y)
{
    terminal_column = x;
    terminal_row = y; 
    
    const size_t pos = terminal_row * VGA_WIDTH + terminal_column;

    i686_outb(0x3D4, 0x0F);
    i686_outb(0x3D5, (uint8_t)(pos & 0xFF));
    i686_outb(0x3D4, 0x0E);
    i686_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
    return pos;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    // size_t tmp_x = terminal_row; 
    // size_t tmp_y = terminal_column;
    
    int index = terminal_setcursor(x, y);
    terminal_buffer[index] = vga_entry(c, color);
}

uint8_t terminal_getchar(size_t x, size_t y)
{
    return (uint8_t)terminal_buffer[ y*VGA_WIDTH + x ];
}
uint8_t terminal_getcolour(size_t x, size_t y)
{
    return (uint8_t)(terminal_buffer[ y*VGA_WIDTH + x ] >> 8);
}

void terminal_putchar(char c)
{
    uint8_t flags = 0b00000000;

    
    if (c == '\n')
    {
        terminal_row++;
        terminal_column = 0;
        flags |= 0b00000001;
    } else if (c == '\t') {
        terminal_column += 2;
        flags |= 0b00000001;
    } else if (c == '\b') {
        // terminal_column -= 1;
        if (terminal_column > 0) { terminal_column--; }
        else { 
            if (terminal_row > 0) { terminal_row--; terminal_column = VGA_WIDTH - 1; }
            else { terminal_column = 0; }
        }
        flags |= 0b00000001;
    }

    if (terminal_column >= VGA_WIDTH)
    {
        terminal_row++;
        terminal_column -= VGA_WIDTH;
    }
    if (terminal_row >= VGA_HEIGHT)
    {
        terminal_scrollback(terminal_row - VGA_HEIGHT);
    }

    int _row = terminal_row;
    int _col = terminal_column;
    
    if (flags) 
    {
        switch (flags)
        {
            case (0b0000000): {
                break;
            }
            case (0b0000001): {
                terminal_putentryat(0, terminal_colour, terminal_column, terminal_row);

                break;
            }
        }
        return;
    }

    terminal_row = _row;
    terminal_column = _col;

    terminal_putentryat(c, terminal_colour, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) 
    {
        terminal_column = 0;
        terminal_row++;
    }
    if (terminal_row >= VGA_HEIGHT)
    {
        // terminal_row = 0;
        terminal_scrollback(terminal_row - VGA_HEIGHT);
    }


    terminal_setcursor(terminal_column, terminal_row);
}


void terminal_scrollback(int lines)
{
    int _row = terminal_row;
    int _col = terminal_column;

    for (int y = lines; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putentryat(terminal_getchar(x, y), terminal_getcolour(x, y), x, y - lines);
        }
    }

    for (int y = VGA_HEIGHT - lines; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            // terminal_putentryat('\0', terminal_colour, x, y);
            terminal_putentryat(0, terminal_getcolour(x, y), x+1, y);
        }
    }

    terminal_row = _row;
    terminal_column = _col;

    terminal_row -= lines;
}

void terminal_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char* data)
{
    terminal_write(data, strlen(data));
}

void terminal_write_left_anchor(const char* buf, size_t x, size_t y)
{
    int _row = terminal_row;
    int _col = terminal_column;

    terminal_setcursor(x, y);
    terminal_writestring(buf);

    terminal_row = _row;
    terminal_column = _col;
    terminal_setcursor(terminal_column, terminal_row);
}

void terminal_write_right_anchor(const char* buf, size_t x, size_t y)
{
    terminal_write_left_anchor(buf, x-strlen(buf), y);
}