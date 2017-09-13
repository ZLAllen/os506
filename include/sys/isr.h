#ifndef _IRQ_H
#define _IRQ_H

#include <sys/defs.h>

void irq_handler(uint64_t num);

extern void _isr0();
extern void _isr32();


#endif
