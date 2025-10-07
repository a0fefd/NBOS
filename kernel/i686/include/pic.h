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

void i686_pic_config(uint8_t offset_pic1, uint8_t offset_pic2);

#endif