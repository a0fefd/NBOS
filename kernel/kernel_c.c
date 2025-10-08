#include "i686/include/isrs.h"
#include "include/kernel.h"
#include "include/tutils.h"
#include "include/keyboard.h"
#include "bash/include/bash.h"
#include "i686/include/gdt.h"
#include "i686/include/idt.h"
#include "i686/include/isr.h"
#include "i686/include/pic.h"
#include "../libc/include/stdio.h"


int kernel_main(void)
{
    /* Setup GDT table */
    i686_gdt_init();   

    /* Setup IDT table */
    i686_idt_init();

    /* Initalise ISR list */
    i686_isr_init();

    /* Initialise terminal interface */
    terminal_init();
    terminal_setcolour(
        vga_entry_colour(
            VGA_COLOUR_WHITE,   // Text colour
            VGA_COLOUR_BLACK    // Background colour
        )
    );
    
    /* Program Start */
    asm volatile ("sti");

    printf("Welcome to NBOS!\n");

    return 0;
}
