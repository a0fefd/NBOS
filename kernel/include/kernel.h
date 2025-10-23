#ifndef _KERNEL_H
#define _KERNEL_H

#include "../i686/include/io.h"

#define SCREEN_W 1024
#define SCREEN_H 768

// #define SCREEN_W 320
// #define SCREEN_H 300
#define SCREEN_AREA (SCREEN_W*SCREEN_H)*3

struct VesaModeInfo {
    uint16_t attributes;
    uint8_t  winA, 
             winB;
    uint16_t granularity;
    uint16_t winsize;
    uint16_t segmentA, 
             segmentB;
    uint32_t realFctPtr;
    uint16_t pitch;
    uint16_t xres, 
             yres;
    uint8_t  xchar, 
             ychar, 
             planes, 
             bpp, 
             banks;
    uint8_t  memoryModel, 
             bankSize, 
             imagePages;
    uint8_t  reserved0;
    uint8_t  redMask, 
             redPos;
    uint8_t  greenMask, 
             greenPos;
    uint8_t  blueMask, 
             bluePos;
    uint8_t  reservedMask, 
             reservedPos;
    uint8_t  directColorModeInfo;
    uint32_t physbase;
    uint32_t offscreenMemoryPtr;
    uint16_t offscreenMemorySize;
    uint8_t __omitted[206];
} __attribute__((packed));

void __attribute__((cdecl)) kernel_main(struct VesaModeInfo* info);

#endif