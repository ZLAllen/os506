#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <sys/defs.h>

//I/O ports


// memory 
void* memsetw(const void* addr, int pattern, uint count);
void* memset(const void* addr, int pattern, uint count);
void* memmove(void* src, void* dst, uint n);
void outb(uint16_t port,uint8_t value);
unsigned char inb(uint16_t port);
void out32(uint32_t port, uint32_t value);
uint32_t in32(uint32_t port);

//asm 
static inline void cr3_w(uint64_t value)
{
    __asm__ volatile("movq %0, %%cr3" :: "r"(value): "memory");
}

static inline uint64_t cr3_r()
{
    uint64_t ret;
    __asm__ volatile("movq %%cr3, %0" :"=r"(ret)::"memory");
    return ret;
}


static inline void cr0_w(uint64_t value)
{
    __asm__ volatile("movq %0, %%cr0" :: "r"(value): "memory");
}

static inline uint64_t cr0_r()
{
    uint64_t ret;
    __asm__ volatile("movq %%cr0, %0" :"=r"(ret)::"memory");
    return ret;
}

// kernel address
#define KERN 0xFFFFFFFF80000000

#define KERNBASE 0xFFFFFFFF80200000


// page
#define PGSIZE 4096
#define VADDR(x) (KERN + (uint64_t)(x))

//error
void panic(char *s);

#endif
