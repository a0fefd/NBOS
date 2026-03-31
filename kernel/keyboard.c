#include "include/keyboard.h"
#include "include/graphics.h"
#include "i686/include/idt.h"
#include "i686/include/io.h"
#include "i686/include/pic.h"
#include "../libc/include/stdio.h"
#include "include/kernel.h"

uint8_t shift = 0;
uint8_t typed = 0;


char kbd_US[128] = {
    0,   27,   '1', '2',  '3',  '4',  '5',  '6', '7',
    '8', '9',  '0', '-',  '=',  '\b', '\t', /* <-- Tab */
    'q', 'w',  'e', 'r',  't',  'y',  'u',  'i', 'o',
    'p', '[',  ']', '\n', 0, /* <-- control key */
    'a', 's',  'd', 'f',  'g',  'h',  'j',  'k', 'l',
    ';', '\'', '`', 0,    '\n', 'z',  'x',  'c', 'v',
    'b', 'n',  'm', ',',  '.',  '/',  0,    '*', 0, /* Alt */
    ' ',                                            /* Space bar */
    0,                                              /* Caps lock */
    0,                                              /* 59 - F1 key ... > */
    0,   0,    0,   0,    0,    0,    0,    0,   0, /* < ... F10 */
    0,                                              /* 69 - Num lock*/
    0,                                              /* Scroll Lock */
    0,                                              /* Home key */
    0,                                              /* Up Arrow */
    0,                                              /* Page Up */
    '-', 0,                                         /* Left Arrow */
    0,   0,                                         /* Right Arrow */
    '+', 0,                                         /* 79 - End key*/
    0,                                              /* Down Arrow */
    0,                                              /* Page Down */
    0,                                              /* Insert Key */
    0,                                              /* Delete Key */
    0,   0,    0,   0,                              /* F11 Key */
    0,                                              /* F12 Key */
    0, /* All other keys are undefined */
};

char kbd_US_lookup[128] = {
    0,
    ['1'] = 2,
    ['2'] = 3,
    ['3'] = 4,
    ['4'] = 5,
    ['5'] = 6,
    ['6'] = 7,
    ['7'] = 8,
    ['8'] = 9,
    ['9'] = 10,
    ['0'] = 11,
    ['-'] = 12,
    ['='] = 13,
    ['q'] = 16,
    ['w'] = 17,
    ['e'] = 18,
    ['r'] = 19,
    ['t'] = 20,
    ['y'] = 21,
    ['u'] = 22,
    ['i'] = 23,
    ['o'] = 24,
    ['p'] = 25,
    ['['] = 26,
    [']'] = 27,
    ['a'] = 30,
    ['s'] = 31,
    ['d'] = 32,
    ['f'] = 33,
    ['g'] = 34,
    ['h'] = 35,
    ['j'] = 36,
    ['k'] = 37,
    ['l'] = 38,
    [';'] = 39,
    ['\''] = 40,
    ['`'] = 41,
    ['z'] = 44,
    ['x'] = 45,
    ['c'] = 46,
    ['v'] = 47,
    ['b'] = 48,
    ['n'] = 49,
    ['m'] = 50,
    [','] = 51,
    ['.'] = 52,
    ['/'] = 53,
    [' '] = 57,
};

int16_t kbd_US_shift_table[128] = {
    0,
    ['!'] = '1'-'!',
    ['@'] = '2'-'@',
    ['#'] = '3'-'#',
    ['$'] = '4'-'$',
    ['%'] = '5'-'%',
    ['^'] = '6'-'^',
    ['&'] = '7'-'&',
    ['*'] = '8'-'*',
    ['('] = '9'-'(',
    [')'] = '0'-')',
    ['_'] = '-'-'_',
    ['+'] = '='-'+',
};

void init_keyboard() {
    typed = 0; shift = 0;
    for (int i = 'A'; i < 'Z'+1; i++)
    {
        kbd_US_shift_table[i] = 32;
        // kbd_US_shift_table[i+32] = -32;
    }
}

void keyboard_main() {
    uint8_t scancode = i686_inb(KEYBOARD_DATA_PORT);
    typed = 1;
    if (scancode == 42)
        shift = !shift;
    else if (scancode == 14)
        graphics_backspace_pixelmap();
    else if (scancode == 29)
    {
        kernel_power_off();
    }
    else if (scancode < 128 )
    {
        printf("%c", kbd_US[scancode]);
        // graphics_draw_pixelmap(kbd_pixelmaps[scancode], 0xffffff);
    }
    typed = 0;

    i686_pic_sendeoi(1);
}
