#ifndef _KERNEL_H
#define _KERNEL_H

#include "../i686/include/io.h"

#define SCREEN_W 1024
#define SCREEN_H 768

// #define SCREEN_W 320
// #define SCREEN_H 300
#define SCREEN_AREA (SCREEN_W*SCREEN_H)*3

struct __attribute__((packed)) VesaModeInfo{
    uint16_t attributes;        // 0x00
    uint8_t  winA, winB;        // 0x02,0x03
    uint16_t granularity;       // 0x04
    uint16_t winsize;           // 0x06
    uint16_t segmentA, segmentB;// 0x08,0x0A
    uint32_t realFctPtr;        // 0x0C
    uint16_t pitch;             // 0x10 (BytesPerScanLine)
    uint16_t Xres;              // 0x12
    uint16_t Yres;              // 0x14
    uint8_t  Wchar, Ychar;      // 0x16,0x17
    uint8_t  planes;            // 0x18
    uint8_t  bpp;               // 0x19 (BitsPerPixel)
    uint8_t  banks;             // 0x1A
    uint8_t  memory_model;      // 0x1B
    uint8_t  bank_size;         // 0x1C
    uint8_t  image_pages;       // 0x1D
    uint8_t  reserved0;         // 0x1E
    uint8_t  red_mask, red_pos; // 0x1F,0x20
    uint8_t  green_mask, green_pos; //0x21,0x22
    uint8_t  blue_mask, blue_pos;   //0x23,0x24
    uint8_t  rsv_mask, rsv_pos;     //0x25,0x26
    uint8_t  directcolor_attrs;     //0x27
    uint32_t physbase;         // 0x28  (PhysBasePtr)
    uint32_t offscreen_ptr;    // 0x2C
    uint16_t offscreen_size;   // 0x30
    uint8_t  reserved1[206];   // fill out to 256 bytes total
};

void __attribute__((cdecl)) kernel_main(volatile struct VesaModeInfo* info);

#endif