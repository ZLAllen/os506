#include <sys/system.h>
#include <sys/kprintf.h>

void panic(char* s)
{
    kprintf("panic: %s\n", s);

    while(1)
        __asm__ volatile ("hlt");
}
