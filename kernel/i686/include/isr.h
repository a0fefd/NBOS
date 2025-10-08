#ifndef _ISR_H
#define _ISR_H

#include "idt.h"
#include "../../../libc/include/stdio.h"

typedef struct registers_t_st{
    uint32_t ds;
    uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax;
    uint32_t interrupt, err;
    uint32_t eip, cs, eflags, esp, ss;
} __attribute__((packed)) registers_t;

typedef void (*isr_handler)(registers_t* regs);

void i686_isr_init();
extern void __attribute__((cdecl)) i686_isr_handler(registers_t* regs);

#endif