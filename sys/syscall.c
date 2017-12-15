#include <syscall.h>

/**
 * Kernel syscall utility functions
 * so making syscalls aren't so tedious
 */

void yield() {
    uint64_t num = SYS_yield;

    syscallArg0(num);

    __asm__ volatile ("int $0x80"
            ::: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 
}

void sleep(uint64_t ms) {

    uint64_t num = SYS_sleep;
    uint64_t ret;
    syscallArg1(num, ms);

    __asm__ volatile ("int $0x80"
            :"=r" (ret)
            :: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 

    yield();
}

uint64_t test(uint64_t arg) {

    uint64_t num = SYS_test;
    uint64_t ret;
    syscallArg1(num, arg);

    __asm__ volatile ("int $0x80"
            :"=r" (ret)
            :: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 

    return ret;
}

pid_t fork() {
    uint64_t num = SYS_fork;
    pid_t ret;

    syscallArg0(num);

    __asm__ volatile ("int $0x80"
            :"=r" (ret)
            :: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 
    return ret;
}

void exit() {
    uint64_t num = SYS_exit;

    syscallArg0(num);

    __asm__ volatile ("int $0x80"
            ::: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 

    yield();
}

ssize_t write(unsigned int fd, const char *buf, size_t count)
{

    return 0;

}

ssize_t read(unsigned int fd, char *buf, size_t count)
{

    return 0;
}

int open(const char *filename, int flags)
{

    return 0;

}

int close(unsigned int fd)
{
    return 0;
}

int getdents(unsigned int fd, struct linux_dirent *d, unsigned int count){
    int ret;
    __asm("syscall"
            :"=a"(ret)
            :"0"(SYS_getdents), "D"(fd), "S"(d), "d"(count)
            :"cc", "rcx", "r11", "memory"
         );

    return ret;
}


void syscallArg0(uint64_t num) {
    __asm__ __volatile__
        ("movq %0, %%rax" :: "r" (num));
}

void syscallArg1(uint64_t num, uint64_t arg0) {
    __asm__ __volatile__
        ("movq %0, %%rax" :: "r" (num));
    __asm__ __volatile__
        ("movq %0, %%rbx" ::"r" (arg0));
}

void syscallArg2(uint64_t num, uint64_t arg0, uint64_t arg1) {
    __asm__ __volatile__
        ("movq %0, %%rax" :: "r" (num));
    __asm__ __volatile__
        ("movq %0, %%rbx;" 
         "movq %1, %%rcx;"
         ::"r" (arg0), "r" (arg1)
        );
}

void syscallArg3(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
    __asm__ __volatile__
        ("movq %0, %%rax" :: "r" (num));
    __asm__ __volatile__
        ("movq %0, %%rbx;" 
         "movq %1, %%rcx;"
         "movq %2, %%rdx;"
         ::"r" (arg0), "r" (arg1), "r" (arg2)
        );
}

void syscallArg4(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
}
void syscallArg5(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4) {
}
