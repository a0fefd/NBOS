#include "i686/include/io.h"
#include "i686/include/pic.h"
#include "include/kernel.h"
#include "../libc/include/stdio.h"

#include "include/tutils.h"
#include "include/memory.h"
// #include "include/keyboard.h"
// #include "bash/include/bash.h"

#include "i686/include/gdt.h"
#include "i686/include/idt.h"
#include "i686/include/isr.h"
#include "i686/include/irq.h"

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

char kbd_US [128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
  '\t', /* <-- Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     
    0, /* <-- control key */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\n', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

void print_scancode()
{
    uint8_t scancode = i686_inb(0x60);

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

    terminal_init(VGA_COLOUR_WHITE, VGA_COLOUR_BLACK);

    
    /* Program Start */
    printf("Welcome to NBOS!\n");
    
    for (;;)
        asm("hlt");

    return;
}
