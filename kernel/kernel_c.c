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
}

void dump_idt(int n)
{
    idt_entry_t e = idt[n];
    uint32_t base = ((uint32_t)e.base_high << 16) | e.base_low;
    printf("idt[%d]: base=%d sel=%d flags=%d\n", n, base, e.selector, e.flags);
}
void dump_pic_masks() {
    uint8_t m1 = i686_inb(0x21);
    uint8_t m2 = i686_inb(0xA1);
    printf("PIC masks: master=%d slave=%d\n", m1, m2);
}
void dump_bytes(uint32_t addr, int n) {
    uint8_t *p = (uint8_t*) addr;
    printf("dump @ %d: ", addr); // printing decimal is fine
    for (int i = 0; i < n; ++i) {
        printf("%d ", p[i]);      // prints each byte in decimal
    }
    printf("\n");
}

void test(registers_t* regs)
{
    printf("?");
}

int kernel_main()
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    kernel_init();
    terminal_init();

    /* Program Start */
    printf("Welcome to NBOS!\n");

    return 0;
}
