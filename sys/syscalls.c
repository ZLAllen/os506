#include <sys/defs.h>
#include <syscalls.h>

void sys_test() {
    
}

/**
 * Supported syscalls
 * Functions defined above
 * Syscall numbers defined in syscalls.h
 */
function syscalls[] = {
    [SYS_test] sys_test
};

/**
 * Syscall handler (int 0x80)
 *
 * Syscall number: rax
 *
 * Params (in order):
 * rbx, rcx, rdx, rsi, rdi, rbp 
 *
 * Return:
 * eax
 */
void syscall(void) {

    uint64_t num;
    function callFunc;

    // TODO - check valid syscall number

    // read syscall number from rax register
    __asm__ __volatile__
        ("movq %%rax, %0"
         :"=r" (num)
         ::);

    // get function associated with syscall
    __asm__ __volatile__(PUSHREGS);
    callFunc = syscalls[num];
    __asm__ __volatile__(POPREGS);

    //test only, TODO - use call instruction
    callFunc();
    
}
