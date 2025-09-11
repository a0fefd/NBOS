#include "include/tutils.h"
#include "include/kernel.h"
#include "../libc/include/stdio.h"

#if !defined (__i386__)
#error "ix86-elf compiler required."
#endif

void kernel_main(void)
{
    /* Initialise terminal interface */
    terminal_initialise();

    const char string[] = "John's OS\n\nMake Sure to SDIYBT :peace:\n";
    size_t string_length = strlen(string);

    VGA_COLOUR starting_col = VGA_COLOUR_LIGHT_BLUE;

    for (size_t i = 0; i < string_length; i++)
    {
        terminal_setcolour(vga_entry_colour(starting_col + i%(16-starting_col), VGA_COLOUR_BLACK));
        putchar(string[i]);
    }

    return;
}