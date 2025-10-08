#include "include/isr.h"

void i686_int_handler()
{

}


void i686_isr_init()
{
    i686_isr_initgates();
    for (int i = 0; i < 256; i++)
        i686_idt_enablegate(i);

    i686_idt_disablegate(0x80);
}