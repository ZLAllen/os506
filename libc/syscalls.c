#include "sysdefs.h"
#include "syscalls.h"

// in x86-64, we can free rcx and r11 for kernel

int read(int fd, char* buf, int size){
    int ret;

    asm volatile
        (
         "syscall"
         : "=a" (ret)
         : "0"(__NR_read), "D"(fd), "S"(buf), "d"(size)
         : "cc", "rcx", "r11", "memory"
        );
    return ret;
}

int write(int fd, const char* buf, int size){
    int ret;
    asm volatile
        (
         "syscall"
         : "=a" (ret)
         : "0"(__NR_write), "D"(fd), "S"(buf), "d"(size)
         : "cc", "rcx", "r11", "memory"
        );
    return ret;
}

int open(const char* file, int flag){
    int ret;
    asm volatile
        (
         "syscall"
         : "=a" (ret)
         : "0"(__NR_open), "D"(file),"S"(flag)
         : "cc", "rcx", "r11"
        );
    return ret;
}

int close(int fd){
   int ret;

   asm volatile
       (
        "syscall"
        : "=a" (ret)
        : "0"(__NR_close), "D"(fd)
        : "cc", "rcx", "r11"
       );
   return ret;
}

void exit(int status){
    asm volatile 
        (
         "syscall"
         :
         : "a"(__NR_exit), "D"(status)
         : "cc"
        );
}
