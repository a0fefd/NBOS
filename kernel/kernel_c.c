#include "include/vga.h"
#include "include/kernel.h"
#include "include/memory.h"
#include "include/tutils.h"
#include "include/graphics.h"
#include "include/keyboard.h"

#include "i686/include/gdt.h"
#include "i686/include/idt.h"
#include "i686/include/isr.h"
#include "i686/include/irq.h"
#include "i686/include/pic.h"

// #include "../libc/include/stdio.h"

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

float t = 0.0f;
float dt = 55.0f/1000.0f;

static inline uint32_t graphics_colour(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 16) || (g << 8) || b;
}

void timer_handler()
{
    float t0 = t;
    t += dt;
    dt = t - t0;

    i686_pic_sendeoi(0);
}

static inline void vga_set_palette_entry(uint8_t idx, uint8_t r, uint8_t g, uint8_t b)
{
    i686_outb(0x3C8, idx);
    i686_outb(0x3C9, r >> 2); // VGA uses 6-bit (0–63), so shift from 8-bit
    i686_outb(0x3C9, g >> 2);
    i686_outb(0x3C9, b >> 2);
}

void stdio_putc(const char c)
{
    while (!((i686_inb(0x3f8 + 5)) & 0x20));
    i686_outb(0x3f8, c);
}
void stdio_write(const char* str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        stdio_putc(str[i]);
    }
}

void __attribute__((cdecl)) kernel_main(struct VesaModeInfo* info)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    kernel_init();

    // vga_set_palette_entry(0, 0, 0, 0);
    // for (int i = 1; i < 64; i++)
    //     vga_set_palette_entry(i, (float)i/63.0f * 255, 0, 0);
    // for (int i = 64; i < 128; i++)
    //     vga_set_palette_entry(i, 0, (float)(i-63)/63.0f * 255, 0);
    // for (int i = 128; i < 192; i++)
    //     vga_set_palette_entry(i, 0, 0, (float)(i-127)/63.0f * 255);
    // for (int i = 1; i < 64; i++)
    // {
    //     uint8_t c = (float)i/63.0f * 255;
    //     vga_set_palette_entry(i+191, c, c, c);
    // }


    // i686_pic_mask(0);
    i686_irq_registerhandler(0, timer_handler);
    i686_irq_registerhandler(1, keyboard_main);
    // i686_irq_registerhandler(1, keyboard_main);

    // text mode not graphics
    // terminal_init(VGA_COLOUR_WHITE, VGA_COLOUR_BLACK);
    
    /* Program Start */
    // printf("Welcome to NBOS!\n");

    // graphics_fillrect(20, 20, 5, 5, graphics_colour(255, 0, 0));

    // for (int i = 0; i < 256; i++)
    // {
    //     // graphics_fillrect_vga(i, 0, 1, SCREEN_H, i);
    //     graphics_fillrect(i, 0, 0, SCREEN_H, graphics_colour(0, i, i));
    // }

    uint32_t* framebuffer = (uint32_t*)info->physbase;
    uint8_t* framebuffer1 = (uint8_t*)0xa0000;
    uint16_t pitch = info->pitch;
    uint8_t  bpp   = info->bpp;

    size_t px = 3*(1*pitch + 1*(bpp/8));
    *(framebuffer+px) = 0xffffff;
    *(framebuffer1+px) = 0xff;
    // for (size_t y = 0; y < SCREEN_H; y++)
    // {
    //     for (size_t x = 0; x < SCREEN_W; x++) 
    //     {
    //     }
    // }

    for (;;)
    {
        
        asm("hlt");
    }
    return;
}
