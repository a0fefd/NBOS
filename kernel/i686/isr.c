#include "include/isr.h"

isr_handler isr_handlers[256];

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

    // printf("Interrupt %d\n", regs->interrupt);
    if (isr_handlers[regs->interrupt] != NULL)
        isr_handlers[regs->interrupt](regs);
    else
        printf("Unhandled interrupt %d\n", regs->interrupt);
}
