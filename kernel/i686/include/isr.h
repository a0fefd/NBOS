#ifndef _ISR_H
#define _ISR_H

#include "idt.h"

void i686_isr_initgates();

void __attribute__((cdecl)) i686_int_handler_wrapper();
extern void __attribute__((cdecl)) i686_int_handler();

#endif