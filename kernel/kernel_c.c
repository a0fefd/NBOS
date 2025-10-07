#include "include/kernel.h"
#include "include/tutils.h"
#include "include/idt.h"
#include "include/keyboard.h"
#include "bash/include/bash.h"
#include "../libc/include/stdio.h"


int kernel_main(void)
{
    /* Initialise terminal interface */
    terminal_initialise();

    /* Setup IDT table */
    idt_install();
    
    /* Initialise keyboard interactions */
    // keyboard_init();
    
    /* Set the colour of the terminal */
    terminal_setcolour(
        vga_entry_colour(
            VGA_COLOUR_WHITE,   // Text colour
            VGA_COLOUR_BLACK    // Background colour
        )
    );
    
    printf("Welcome to NBOS!\n");
    
    char input_buffer[256];
    uint8_t input_buffer_idx = 0;
    
    asm volatile ("sti");

    return 0;
}
