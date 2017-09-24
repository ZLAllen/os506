#include <sys/system.h>









/******************* MEMORY functions ************************/
void* memmove(void* src, void* dst, uint n){
    char*a = dst, *b = src;
    //handle overlap
    if(src>dst && src+n < dst){
        a = a+n;
        b = b+n;
        while(n-- > 0)
            *--a = *--b;
    }else{
        while(n-- > 0)
            *a++ = *b++;
    }

    return dst;
}

void* memsetw(const void* addr, int pattern, uint count){
    void* ret;
    __asm__ volatile
        ("cld; rep stosw"
            :"=D"(ret)
            :"D"(addr), "a"(pattern), "c"(count)
            :"cc", "memory"
         );

    return ret;
}

void* memset(const void* addr, int pattern, uint count){
    void* ret;
    __asm("cld; rep stosb"
            :"=D"(ret)
            :"D"(addr), "a"(pattern), "c"(count)
            :"cc", "memory"
         );

    return ret;
}

/******************* IO functions ************************/


//follow GAS format
void outb(uint16_t port,uint8_t value) {
    
    __asm__ volatile ("out %0, %1" : : "a"(value), "d"(port));
}

unsigned char inb(uint16_t port){
    unsigned char value;
    __asm__ volatile ("in %1, %0" :"=a"(value): "d"(port));
    return value;
}

void out32(uint32_t port, uint32_t value) 
{
    __asm__ volatile ("outl %0, %w1" : : "a"(value), "Nd"(port));
}

uint32_t in32(uint32_t port)
{
    uint32_t value;
    __asm__ volatile ("inl %w1, %0" :"=a"(value): "Nd"(port));
    return value;
}
