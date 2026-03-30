#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "../../libc/include/stdint.h"

#define KEYBOARD_DATA_PORT 0x60
#define BUFFER_SIZE 256

extern char kbd_US[128];
extern char kbd_US_lookup[128];
extern int16_t kbd_US_shift_table[128];
extern uint8_t shift;
extern uint8_t typed;

typedef struct KEY_EVENT_ST {
    uint8_t code;
} KEY_EVENT;

void init_keyboard();
void keyboard_main();

#endif
