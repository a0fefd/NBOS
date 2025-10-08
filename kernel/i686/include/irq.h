#ifndef _IRQ_H
#define _IRQ_H

#include "isr.h"

typedef void (*irq_handler)(registers_t* regs);

void i686_irq_init();
void i686_irq_registerhandler(int intcode, irq_handler handler);

#endif