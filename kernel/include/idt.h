#ifndef _IDT_H
#define _IDT_H

#include "../../libc/include/stdint.h"

// One entry in the IDT
typedef struct {
    uint16_t address_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t address_mid;
    uint32_t address_high;
    uint32_t reserved;
} __attribute__((packed)) idt_entry_t;

void set_idt_entry(uint8_t vector, void* handler, uint8_t dpl);
void idt_install(void);

#endif
