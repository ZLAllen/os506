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
