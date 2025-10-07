#include "include/pic.h"
#include "include/io.h"

void i686_pic_config(uint8_t offset_pic1, uint8_t offset_pic2)
{
    outb(PIC1_PORT_COMMAND, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    i686_iowait();
    outb(PIC2_PORT_COMMAND, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    i686_iowait();
}