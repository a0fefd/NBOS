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

void __attribute__((cdecl)) kernel_main()
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    kernel_init();
    terminal_init();

    
    /* Program Start */
    printf("Welcome to NBOS!\n");
    
    // i686_irq_registerhandler(0, test);

    for (;;) {
        asm("hlt");
    }

    return;
}
