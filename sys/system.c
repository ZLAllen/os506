#include <sys/system.h>

void* memmove(void* src, void* dst, uint n){
    char*a = src, *b = dst;
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

//follow GAS format
void outb(uint16_t port,uint8_t value) {
    
    __asm__ volatile ("out %0, %1" : : "a"(value), "d"(port));
}

unsigned char inb(uint16_t port){
    unsigned char value;
    __asm__ volatile ("in %1, %0" :"=a"(value): "d"(port));
    return value;
}
