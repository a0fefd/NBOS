#include "include/tutils.h"
#include "include/kernel.h"
#include "bash/include/bash.h"
#include "../libc/include/stdio.h"


int kernel_main(void)
{
    /* Initialise terminal interface */
    terminal_initialise();

    /* Set the colour of the terminal */
    terminal_setcolour(
        vga_entry_colour(
            VGA_COLOUR_WHITE,   // Text colour
            VGA_COLOUR_BLACK    // Background colour
        )
    );

    printf("Welcome to NBOS!\n");

    bash_execute(bash_cmd_echo("john sigma\n"));

    return 0;
}
