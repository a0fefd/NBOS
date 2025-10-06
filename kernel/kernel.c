#include "include/tutils.h"
#include "include/kernel.h"
#include "../libc/include/stdio.h"

#if !defined (__i386__)
#error "ix86-elf compiler required."
#endif

int kernel_main(void)
{
    /* Initialise terminal interface */
    terminal_initialise();

    VGA_COLOUR starting_col = VGA_COLOUR_LIGHT_BLUE;
    terminal_setcolour(vga_entry_colour(starting_col, VGA_COLOUR_BLACK));

    printf("Welcome to NBOS!\n");

    return 0;
}
