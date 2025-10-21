#include "i686/include/io.h"
#include "i686/include/pic.h"
#include "include/kernel.h"
#include "../libc/include/stdio.h"

#include "include/tutils.h"
#include "include/memory.h"
#include "include/keyboard.h"
// #include "bash/include/bash.h"

#include "i686/include/gdt.h"
#include "i686/include/idt.h"
#include "i686/include/isr.h"
#include "i686/include/irq.h"
#include "include/vga.h"

extern uint8_t __bss_start;
extern uint8_t __end;

void kernel_init()
{
    i686_gdt_init();   
    i686_idt_init();
    i686_isr_init();
    i686_irq_init();
    i686_sti();
}

void test(registers_t* regs)
{
    printf("?");
}

void print_scancode()
{
    uint8_t scancode = i686_inb(KEYBOARD_DATA_PORT);

    if (scancode > 0x80 /*128*/) { return; }

    char character = kbd_US[scancode];    

    if (character == 0) { return; }

    // printf("Code %d: Key %c\n", scancode, character);
    printf("%c", kbd_US[scancode]);
}

void __attribute__((cdecl)) kernel_main()
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    kernel_init();

    i686_pic_mask(0);
    i686_irq_registerhandler(1, print_scancode);
    // i686_irq_registerhandler(1, keyboard_main);

    terminal_init(VGA_COLOUR_WHITE, VGA_COLOUR_BLACK);
    
    /* Program Start */
    printf("Welcome to NBOS!\n");
    for (;;)
        asm("hlt");

    return;
}
