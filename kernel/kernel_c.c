#include "include/kernel.h"
#include "include/tutils.h"
#include "include/keyboard.h"
#include "bash/include/bash.h"
#include "i686/include/gdt.h"
#include "i686/include/idt.h"
#include "i686/include/pic.h"
#include "../libc/include/stdio.h"


int kernel_main(void)
{
    /* Initialise terminal interface */
    terminal_initialise();

    /* Setup GDT table */
    i686_gdt_init();

    /* Setup IDT table */
    i686_idt_init();
    
    /* Setup PIC */
    // i686_pic_config(uint8_t offset_pic1, uint8_t offset_pic2)

    /* Initialise keyboard interactions */
    // keyboard_init();
    
    /* Set the colour of the terminal */
    terminal_setcolour(
        vga_entry_colour(
            VGA_COLOUR_WHITE,   // Text colour
            VGA_COLOUR_BLACK    // Background colour
        )
    );
    
    printf("Welcome to NBOS!!\n");

    asm volatile ("sti");

    return 0;
}
