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
int memcmp(const void *, const void *, uint);
void *memcpy(const void *src, void *dst, uint count);
void *memchr(const void *str, int ch, uint count);


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

static inline void invlpg(uint64_t addr)
{
    __asm__ volatile("invlpg (%0)"::"r"(addr):"memory");
}

// kernel address
#define KERN 0xFFFFFFFF80000000

#define KERNBASE 0xFFFFFFFF80200000


// define a user stack starting point for virtual address
#define USER_STACK_TOP 0xF000000000  //note test this address first, adjustable
#define USER_STACK_SIZE 0x8000  //8 pages


// page
#define PGSIZE 4096
#define VADDR(x) (KERN + (uint64_t)(x))

//error
void panic(char *s);


#endif
