#ifndef _IDT_H
#define _IDT_H

#include "../../../utils/include/butils.h"
#include "includestdint.h"

typedef struct {
    uint16_t base_low;
    uint16_t selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t size;
    idt_entry_t* offset;
} __attribute__((packed)) idt_descriptor_t;

typedef enum IDT_FLAGS_EN {
    IDT_FLAG_GATE_TASK          = 0x5,
    IDT_FLAG_GATE_16BIT_INT     = 0x6,
    IDT_FLAG_GATE_16BIT_TRAP    = 0x7,
    IDT_FLAG_GATE_32BIT_INT     = 0xe,
    IDT_FLAG_GATE_32BIT_TRAP    = 0xf,

    IDT_FLAG_RING0              = ( 0 << 5 ),
    IDT_FLAG_RING1              = ( 1 << 5 ),
    IDT_FLAG_RING2              = ( 2 << 5 ),
    IDT_FLAG_RING3              = ( 3 << 5 ),

    IDT_FLAG_PRESENT            = 0x80,
} IDT_FLAGS;

#define idt_dt_t idt_descriptor_t

void i686_idt_init();
// extern void idt_load(idt_dt_t* idt_desc);
void i686_idt_setgate(int intcode, void* base, uint16_t selector, uint8_t flags);
void i686_idt_enablegate(int intcode);
void i686_idt_disablegate(int intcode);
void __attribute__((cdecl)) i686_idt_load(idt_dt_t* idt_desc);

#endif
