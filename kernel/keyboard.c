#include "include/keyboard.h"
#include "include/idt.h"
#include "include/ports.h"

#include "../libc/include/stdio.h"

volatile char key_buffer[BUFFER_SIZE];
volatile int buf_index = 0;

static char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t', 'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, // Control
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, // Left shift
    '\\','z','x','c','v','b','n','m',',','.','/', 0, // Right shift
    '*',
    0,  // Alt
    ' ', // Spacebar
    0,  // Caps lock
    // etc...
};

void keyboard_handler(void) 
{
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // key_buffer[buf_index] = scancode;
    // buf_index = (buf_index + 1) % BUFFER_SIZE;

    if (scancode)
        printf("Read: %s\n", scancode);
}

char keyboard_getchar(void) 
{
    return 0;
}

void keyboard_init() 
{
    // You'll want to do other setup here in your own driver:
    // ensure the input buffer of the keyboard is empty, check which scancode
    // set is in use, enable irqs.
}