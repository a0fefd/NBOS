#ifndef _PIC_H
#define _PIC_H

#include "includestdint.h"

#define PIC1_PORT_COMMAND   0x20
#define PIC1_PORT_DATA      0x21
#define PIC2_PORT_COMMAND   0xa0
#define PIC2_PORT_DATA      0xa1

typedef enum PIC_ICW1_EN {
    PIC_ICW1_ICW4       = 0x1,
    PIC_ICW1_SINGLE     = 0x2,
    PIC_ICW1_INTERVAL4  = 0x4,
    PIC_ICW1_LEVEL      = 0x8,
    PIC_ICW1_INIT       = 0x10,
} PIC_ICW1;

typedef enum PIC_ICW4_EN {
    PIC_ICW4_8086       = 0x1,
    PIC_ICW4_AUTO_EOI   = 0x2,
    PIC_ICW4_BUFFER_MAS = 0x4, // master
    PIC_ICW4_BUFFER_SLV = 0x0, // slave
    PIC_ICW4_BUFFERRED  = 0x8,
    PIC_ICW4_SFNM       = 0x10,
} PIC_ICW4;

typedef enum PIC_CMD_EN {
    PIC_CMD_EOI         = 0x20,
    PIC_CMD_READ_IRR    = 0xa,
    PIC_CMD_READ_ISR    = 0xb,

} PIC_CMD;

void i686_pic_config(uint8_t offset_pic1, uint8_t offset_pic2);
void i686_pic_mask(int irq);
void i686_pic_unmask(int irq);
void i686_pic_disable();
void i686_pic_sendeoi(int irq);
uint16_t i686_pic_get_irr();
uint16_t i686_pic_get_isr();

#endif