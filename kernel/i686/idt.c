#include "include/idt.h"

#define IDT_ENTRIES 256

idt_entry_t idt[IDT_ENTRIES];
idt_dt_t idtr = {
    sizeof(idt) - 1,
    idt
};

void i686_idt_setgate(int intcode, void* base, uint16_t selector, uint8_t flags)
{
    idt[intcode].base_low = ( (uint32_t) base ) & 0x0000ffff;
    idt[intcode].selector = selector;
    idt[intcode].reserved = 0;
    idt[intcode].flags = flags;
    idt[intcode].base_high = ( (uint32_t) base >> 16 ) & 0x0000ffff;
}

void i686_idt_enablegate(int intcode) 
{
    FLAG_SET(idt[intcode].flags, IDT_FLAG_PRESENT);
}

void i686_idt_disablegate(int intcode)
{
    FLAG_UNSET(idt[intcode].flags, IDT_FLAG_PRESENT);
}

void i686_idt_init()
{
    i686_idt_load(&idtr);
}