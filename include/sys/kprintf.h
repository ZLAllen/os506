#ifndef __KPRINTF_H
#define __KPRINTF_H


#include <sys/defs.h>

void kprintf(const char *fmt, ...);
void kputchar(const char c);
void kputs(const char* str);

void update_time(uint32_t day, uint32_t sec);

#endif
