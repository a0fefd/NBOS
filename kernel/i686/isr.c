#include "include/isr.h"
#include "include/idt.h"
#include "include/io.h"

isr_handler isr_handlers[256];

static const char* const exceptions[] = {
    "DIV 0 ERROR",
    "DEBUG EXCEPTION",
    "NON-MASKABLE INT",
    "BREAKPOINT",
    "OVERFLOW ERROR",
    "BOUND RANGE EXCEEDED",
    "INVALID (UNDEF) OPCODE",
    "DEVICE NOT AVAILABLE",
    "DOUBLE FAULT",
    "COPROCESSOR SEGMENT OVERRUN", // reserved
    "INVALID TSS",
    "SEGMENT NOT PRESENT",
    "STACK SEGMENT FAULT",
    "GENERAL PROTECTION",
    "PAGE FAULT", 
    "", // intel reserved
    "x87 FPU FLOATINGPOINT ERROR",
    "ALIGNMENT CHECK",
    "MACHINE CHECK",
    "SIMD FLOATINGPOINT EXCEPTION",
    "VIRUALIZATION EXCEPTION",
    "CONTROL PROTECTION EXCEPTION",
    
    "", /* reserved */
    "", 
    "", 
    "", 
    "", 
    "", 
    "", 
    "", 
    "", 
    "", /* reserved */

    /* external ints */

};

void i686_isr_initgates();

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
    else if (regs->interrupt >= 32)
        printf("Unhandled interrupt %d\n", regs->interrupt);
    else
    {
        printf("Unhandled exception %d: %s\n", regs->interrupt, exceptions[regs->interrupt]);

        printf("eax=%d; ebx=%d; ecx=%d; edx=%d; \ 
              \nerr=%d;", 
                
                regs->eax, regs->ebx, regs->ecx, regs->edx,
                regs->err

        );
        i686_hang();
    }
}

void i686_isr_registerhandler(int intcode, isr_handler handler)
{
    isr_handlers[intcode] = handler;
    i686_idt_enablegate(intcode);
}
