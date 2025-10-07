#ifndef _PORTS_H
#define _PORTS_H

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char val);

#endif
