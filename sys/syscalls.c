#include <sys/defs.h>
#include <syscalls.h>
#include <sys/kprintf.h>

void sys_test(uint64_t testArg) {
   kprintf("print me. Argument is %d\n", testArg);
   while(1);
}

int sys_getdents(unsigned int fd, struct linux_dirent* dirp, unsigned int count)
{


}


/**
 * Supported syscalls
 * Functions defined above
 * Syscall numbers defined in syscalls.h
 * Number indicates how many arguments function requires
 */
functionWithArg syscalls[] = {
    [SYS_test] {1, sys_test}
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
 * rax
 */
void syscall(void) {

    uint64_t num;
    functionWithArg callFunc;
    uint64_t arg0, arg1, arg2, arg3, arg4;

    // read syscall number from rax register
    __asm__ __volatile__(
        "movq %%rax, %0;"
         :"=r" (num)
         ::"%rbx", "%rcx", "%rdx", "%rsi", "%rdi" // these registers must not change
    );

    __asm__ __volatile__(PUSHREGS);
    kprintf("Performing syscall %d\n", num);
    __asm__ __volatile__(POPREGS);

    // read arguments from registers
    __asm__ __volatile__(
            "movq %%rbx, %0;"
            "movq %%rcx, %1;"
            "movq %%rdx, %2;"
            "movq %%rsi, %3;"
            "movq %%rdi, %4;"
            :"=r" (arg0), "=r" (arg1), "=r" (arg2), "=r" (arg3), "=r" (arg4)
        );

    // get function associated with syscall
    callFunc = syscalls[num];

    switch (callFunc.count) {
        case 0:
            callFunc.func();
            break;
        case 1:
            callFunc.func(arg0);
            break;
        case 2:
            callFunc.func(arg0, arg1);
            break;
        case 3:
            callFunc.func(arg0, arg1, arg2);
            break;
        case 4:
            callFunc.func(arg0, arg1, arg2, arg3);
            break;
        case 5:
            callFunc.func(arg0, arg1, arg2, arg3, arg4);
            break;
    }
}
