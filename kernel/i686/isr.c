#include "include/isr.h"
#include "include/idt.h"
// #include "include/isrs.h"

extern void i686_isr_initgates();

void i686_isr_init()
{
    i686_isr_initgates();
    for (int i = 0; i < 256; i++)
        i686_idt_enablegate(i);


    i686_idt_disablegate(0x80);
}

void __attribute__((cdecl)) i686_isr_handler(registers_t* regs)
{
    printf("Interrupt %d\n", regs->interrupt);
}
