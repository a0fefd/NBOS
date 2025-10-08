#ifndef _IO_H
#define _IO_H

#include "includestdint.h"

#define PORT_UNUSED 0x80

void                           i686_iowait();
uint8_t __attribute__((cdecl)) i686_inb(uint16_t port);
void    __attribute__((cdecl)) i686_outb(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) i686_sti();
uint8_t __attribute__((cdecl)) i686_cli();
void                           i686_hang();
void    __attribute__((cdecl)) i686_hang_s();

#endif