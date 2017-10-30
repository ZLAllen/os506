#ifndef __KPRINTF_H
#define __KPRINTF_H


#include <sys/defs.h>

void kprintf(const char *fmt, ...);
void kputchar(const char c);
void kputs(const char* str);
void clr();

void update_time(uint32_t day, uint32_t sec);

// defines

#define BASE (KERN + 0xb8000)


#endif
