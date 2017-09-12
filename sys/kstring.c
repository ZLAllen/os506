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
    __asm("cld; rep stosw"
            :"=D"(ret)
            :"D"(addr), "a"(pattern), "c"(count)
            :"cc", "memory"
         );

    return ret;
}
