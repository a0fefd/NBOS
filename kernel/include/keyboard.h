#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "../../libc/include/stdint.h"

#define KEYBOARD_DATA_PORT 0x60
#define BUFFER_SIZE 256

typedef struct KEY_EVENT_ST {
    uint8_t code;
} KEY_EVENT;

void keyboard_init();

#endif