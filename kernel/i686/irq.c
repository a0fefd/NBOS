#include "include/irq.h"
#include "include/pic.h"
#include "include/io.h"

#define PIC_REMAP_OFFSET 0x20

irq_handler irq_handlers[16];

void i686_irq_handler(registers_t* regs)
{
    int irq = regs->interrupt - PIC_REMAP_OFFSET;

    uint8_t pic_isr = i686_pic_get_isr();
    uint8_t pic_irr = i686_pic_get_irr();

    if (irq_handlers[irq] != NULL)
    {
        irq_handlers[irq](regs);
    } else {
        printf("Unhandled irq %d: isr=%d; irr=%d.\n", irq, pic_isr, pic_irr);
    }

    i686_pic_sendeoi(irq);
}

void i686_irq_init()
{
    i686_cli();
    i686_pic_config(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8);

    for (int i = 0; i < 16; i++)
        i686_isr_registerhandler(PIC_REMAP_OFFSET + i, i686_irq_handler);

    // i686_sti();
}

void i686_irq_registerhandler(int irq, irq_handler handler)
{
    irq_handlers[irq] = handler;
}