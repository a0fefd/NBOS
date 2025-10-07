#include "include/idt.h"

#define IDT_ENTRIES 256

idt_entry_t idt[IDT_ENTRIES];
// idt_ptr_t idtp;

// This function is in assembly
extern void idt_load(uint32_t);


void set_idt_entry(uint8_t vector, void* handler, uint8_t dpl)
{
    uint32_t handler_addr = (uint32_t)handler;

    idt_entry_t* entry = &idt[vector];
    entry->address_low = handler_addr & 0xFFFF;
    entry->address_mid = (handler_addr >> 16) & 0xFFFF;
    // entry->address_high = handler_addr >> 32;
    //your code selector may be different!
    entry->selector = 0x8;
    //trap gate + present + DPL
    entry->flags = 0b1110 | ((dpl & 0b11) << 5) |(1 << 7);
    //ist disabled
    entry->ist = 0;
}

// Initialize and load IDT
void idt_install(void) {
    
}
