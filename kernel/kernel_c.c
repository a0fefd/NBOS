#include "include/vga.h"
#include "include/kernel.h"
#include "include/memory.h"
#include "include/tutils.h"
#include "include/graphics.h"
#include "include/paging.h"
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

static uint8_t on = 1;
void kernel_power_off()
{
    on = 0;
}

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

typedef void (*voidfptr_t)(void);

static const char hex_characters[] = "0123456789abcdef";
static void printf_uint32_t_hex(uint32_t number, void (*__func_printf)(const char* str, ...))
{
    for (int i = 3; i >= 0; i--)  // print from highest byte to lowest
    {
        uint8_t byte = (number >> (i * 8)) & 0xFF;
        __func_printf("%c", hex_characters[(byte >> 4) & 0xF]);  // high nibble
        __func_printf("%c", hex_characters[byte & 0xF]);         // low nibble
    }
}
static void printf_uint16_t_hex(uint16_t number, void (*__func_printf)(const char* str, ...))
{
    for (int i = 1; i >= 0; i--)  // print from highest byte to lowest
    {
        uint8_t byte = (number >> (i * 8)) & 0xFF;
        __func_printf("%c", hex_characters[(byte >> 4) & 0xF]);  // high nibble
        __func_printf("%c", hex_characters[byte & 0xF]);         // low nibble
    }
}
static void printf_uint8_t_hex(uint8_t number, void (*__func_printf)(const char* str, ...))
{
    uint8_t byte = number & 0xFF;
    __func_printf("%c", hex_characters[(byte >> 4) & 0xF]);  // high nibble
    __func_printf("%c", hex_characters[byte & 0xF]);         // low nibble
}

void hexdump(uint8_t* addr, size_t n, void (*__func_printf)(const char* str, ...))
{
    static const uint8_t w = 32;
    __func_printf("-- addr: %x, n: %u, w: 32", addr, n);
    for (size_t i = 0; i < n; i++) {
        ((uint32_t)i % w == 0) ? __func_printf("\n   ") : NULL;
        printf_uint8_t_hex(*(addr+i), __func_printf);
        __func_printf(" ");
    }
    __func_printf("\n--\n");
}
void hexdump_full(uint8_t* ptr, void (*__func_printf)(const char* str, ...))
{
    hexdump(ptr, sizeof(*ptr), __func_printf);
}

// #define VRAM_ADDR 0xe0000000
void __attribute__((cdecl)) kernel_main(volatile struct VesaModeInfo* info)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    kernel_init();

    i686_irq_registerhandler(0, timer_handler);

    init_graphics(info);
    init_keyboard();

    i686_irq_registerhandler(1, keyboard_main);

    /* Program Start */
    printf("ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n!@#$%^&*()_+\n\n");
    set_fg_colour(0xa0fefd);
    printf("Welcome to NBOS!\n");
    set_fg_colour(0xffffff);

    while (on)
    {
        asm("hlt");
    }

    // outw(0x604, 0x2000);

    return;
}
