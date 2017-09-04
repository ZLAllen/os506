#include "syscalls.h"
#define __NR_write 1

int write(int fd, const char* buf, int count){
    int ret;
    asm volatile
        (
         "syscall"
         : "=a" (ret)
         : "0"(__NR_write), "D"(fd), "S"(buf), "d"(count)
         : "cc", "rcx", "r11", "memory"
        );
    return ret;
}
