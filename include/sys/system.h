#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <sys/defs.h>

//I/O ports


// memory 
void* memsetw(const void* addr, int pattern, uint count);
void* memmove(void* src, void* dst, uint n);
void outb(uint16_t port,uint8_t value);
unsigned char inb(uint16_t port);
void out32(uint32_t port, uint32_t value);
uint32_t in32(uint32_t port);
#endif
