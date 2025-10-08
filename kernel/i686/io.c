#include "include/io.h"

#include "../../libc/include/stdio.h"
#include "../include/tutils.h"

void i686_iowait()
{
    i686_outb(PORT_UNUSED, 0);
}

void i686_hang()
{
    terminal_setcolour(
        vga_entry_colour(VGA_COLOUR_RED, VGA_COLOUR_BLACK)
    );
    printf("\n\nCPU HUNG\n\n");
    i686_hang_s();
}