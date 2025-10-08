#include "include/isr.h"

void __attribute__((cdecl)) i686_isr_handler(registers_t* regs)
{
    printf("Interrupt %c\n", regs->interrupt);
}
