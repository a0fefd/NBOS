#include "i686/include/io.h"
#include "i686/include/pic.h"
#include "include/kernel.h"
#include "../libc/include/stdio.h"

#include "include/tutils.h"
#include "include/memory.h"
#include "include/keyboard.h"
// #include "bash/include/bash.h"

#include "i686/include/gdt.h"
#include "i686/include/idt.h"
#include "i686/include/isr.h"
#include "i686/include/irq.h"
#include "include/vga.h"

extern uint8_t __bss_start;
extern uint8_t __end;

void kernel_init()
{
    i686_gdt_init();   
    i686_idt_init();
    i686_isr_init();
    i686_irq_init();
    i686_sti();
}

// void print_scancode()
// {
//     uint8_t scancode = i686_inb(KEYBOARD_DATA_PORT);

//     if (scancode > 0x80 /*128*/) { return; }

//     char character = kbd_US[scancode];    

//     if (character == 0) { return; }

//     // printf("Code %d: Key %c\n", scancode, character);
//     printf("%c", kbd_US[scancode]);
// }

uint8_t* vram = (uint8_t*)0xA0000;
float t = 0.0f;
float dt = 55.0f/1000.0f;

#define SCREEN_W 320
#define SCREEN_H 300
#define SCREEN_AREA (SCREEN_W*SCREEN_H)

static void graphics_fillrect(size_t x, size_t y, size_t w, size_t h, VGA_COLOUR colour)
{
    for (size_t j = 0; j < h; j++)
    {
        for (size_t i = 0; i < w; i++)
        {
            *(vram + SCREEN_W*(y+j) + x+i) = colour;
        }
    }
}

static void graphics_clrscr()
{
    for (size_t i = 0; i < SCREEN_AREA; i++)
    {
        *(vram + i) = VGA_COLOUR_BLACK;
    }
}

void timer_handler()
{
    float t0 = t;
    t += dt;
    dt = t - t0;

    i686_pic_sendeoi(0);
}

uint8_t inputbuffer = 0;

void input_handler_next()
{
    uint16_t code = i686_inb(KEYBOARD_DATA_PORT);

    char thing[5] = {
        0,'w','a','s','d'
    };
    if (code >= 0x80) 
    {
        inputbuffer = 0;
        // return;
    }

    switch (kbd_US[code])
    {
        case 'w':
            inputbuffer = 1;
            break;

        case 'a':
            inputbuffer = 2;
            break;
        
        case 's':
            inputbuffer = 3;
            break;
        
        case 'd':
            inputbuffer = 4;
            break;
        
        default:
            break;

    }
    return;
}
void input_handler()
{
    input_handler_next();
    i686_pic_sendeoi(1);
}
void __attribute__((cdecl)) kernel_main()
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    kernel_init();

    // i686_pic_mask(0);
    i686_irq_registerhandler(0, timer_handler);
    i686_irq_registerhandler(1, input_handler);
    // i686_irq_registerhandler(1, keyboard_main);

    // text mode not graphics
    // terminal_init(VGA_COLOUR_WHITE, VGA_COLOUR_BLACK);
    
    /* Program Start */
    // printf("Welcome to NBOS!\n");

    int x,y = 0;
    float vx, vy = 0;

    float vel[5] = {
        0,
        -0.1f,
        -0.1f,
        0.1f,
        0.1f,
    };

    for (;;)
    {
        graphics_fillrect(x, y, 5, 5, VGA_COLOUR_BLACK);

        if (inputbuffer % 2 == 0 && inputbuffer > 0)
        {
            vx = vel[inputbuffer];
        } else if (inputbuffer > 0) {
            vy = vel[inputbuffer];
        } else {
            vx = vy = 0;
        }

        x += 320.0f*vx*dt;
        y += 300.0f*vy*dt;
        graphics_fillrect(x, y, 5, 5, VGA_COLOUR_WHITE);
        asm("hlt");
    }
    return;
}
