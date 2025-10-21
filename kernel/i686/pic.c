#include "include/pic.h"
#include "include/io.h"

void i686_pic_config(uint8_t offset_pic1, uint8_t offset_pic2)
{
    i686_outb(PIC1_PORT_COMMAND, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    i686_iowait();
    i686_outb(PIC2_PORT_COMMAND, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    i686_iowait();

    i686_outb(PIC1_PORT_DATA, offset_pic1);
    i686_iowait();
    i686_outb(PIC2_PORT_DATA, offset_pic2);
    i686_iowait();

    i686_outb(PIC1_PORT_DATA, 0x4);
    i686_iowait();
    i686_outb(PIC2_PORT_DATA, 0x2);
    i686_iowait();

    i686_outb(PIC1_PORT_DATA, PIC_ICW4_8086);
    i686_iowait();
    i686_outb(PIC2_PORT_DATA, PIC_ICW4_8086);
    i686_iowait();

    i686_outb(PIC1_PORT_DATA, PIC_ICW4_AUTO_EOI);
    i686_iowait();
    i686_outb(PIC2_PORT_DATA, PIC_ICW4_AUTO_EOI);
    i686_iowait();

    i686_outb(PIC1_PORT_DATA, 0);
    i686_iowait();
    i686_outb(PIC2_PORT_DATA, 0);
    i686_iowait();
}

void i686_pic_mask(int irq)
{
    uint8_t port;
    if (irq < 8)
    {
        port = PIC1_PORT_DATA;
    } else {
        port = PIC2_PORT_DATA;
        irq -= 8;
    }
    // uint8_t mask = i686_inb(port);
    // i686_outb(port, mask | ( 1 << irq ) );
    uint8_t mask = i686_inb(port);
    i686_outb(port, mask | ( 1 << irq ) );
}

void i686_pic_unmask(int irq)
{
    uint8_t port;
    if (irq < 8)
    {
        port = PIC1_PORT_DATA;
    } else {
        port = PIC2_PORT_DATA;
        irq -= 8;
    }
    // uint8_t mask = i686_inb(port);
    // i686_outb(port, mask & ~( 1 << irq ) );
    uint8_t mask = i686_inb(port);
    i686_outb(port, mask & ~( 1 << irq ) );
}

void i686_pic_disable()
{
    i686_outb(PIC1_PORT_DATA, 0xff);
    i686_iowait();
    i686_outb(PIC2_PORT_DATA, 0xff);
    i686_iowait();
}

void i686_pic_sendeoi(int irq)
{
    if (irq >= 8)
        i686_outb(PIC2_PORT_COMMAND, PIC_CMD_EOI);
    i686_outb(PIC1_PORT_COMMAND, PIC_CMD_EOI);
}

// uint16_t i686_pic_read_irqrequestregister()
// {
//     i686_outb(PIC1_PORT_COMMAND, PIC_CMD_READ_IRR);
//     i686_outb(PIC2_PORT_COMMAND, PIC_CMD_READ_IRR);
//     return ((uint16_t)i686_inb(PIC1_PORT_COMMAND)) | ( ((uint16_t)i686_inb(PIC2_PORT_COMMAND)) << 8 );
// }

// uint16_t i686_pic_read_inserviceregister()
// {
//     i686_outb(PIC1_PORT_COMMAND, PIC_CMD_READ_ISR);
//     i686_outb(PIC2_PORT_COMMAND, PIC_CMD_READ_ISR);
//     return ((uint16_t)i686_inb(PIC1_PORT_COMMAND)) | ( ((uint16_t)i686_inb(PIC2_PORT_COMMAND)) << 8 );
// }

static uint16_t i686_pic_get_irq_reg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    i686_outb(PIC1_PORT_COMMAND, ocw3); i686_iowait();
    i686_outb(PIC2_PORT_COMMAND, ocw3); i686_iowait();
    return (i686_inb(PIC2_PORT_COMMAND) << 8) | i686_inb(PIC2_PORT_COMMAND);
}

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t i686_pic_get_irr(void)
{
    return i686_pic_get_irq_reg(PIC_CMD_READ_IRR);
}

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t i686_pic_get_isr(void)
{
    return i686_pic_get_irq_reg(PIC_CMD_READ_ISR);
}
