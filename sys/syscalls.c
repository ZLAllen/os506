#include <sys/defs.h>
#include <sys/syscalls.h>
#include <sys/schedule.h>
#include <sys/kprintf.h>
#include <sys/dirent.h>

/** current process (sys/schedule.c) */
extern task_struct *current;

uint64_t sys_test(uint64_t testArg) {
    __asm__ __volatile__(PUSHREGS);
   kprintf("print me. Argument is %d\n", testArg);
    __asm__ __volatile__(POPREGS);
   return 9001;
}
/*
int sys_getdents(unsigned int fd, struct linux_dirent* dirp, unsigned int count)
{
    return 0;
}
*/
/**
 * Fork current process
 * Creates new process as a child of the current
 */
uint64_t sys_fork() {

    // create child process
    task_struct *child = fork_process(current);

    // schedule new process like any other
    schedule(child, current->rsp);

    // return child PID to the parent
    return child->pid;
}


uint64_t sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count) 
{
    if(!dirp || count <= 0)
        return -1;

    return (uint64_t) getdents(fd, dirp, count);//num bytes read is returned
}



/**
 * Supported syscalls
 * Functions defined above
 * Syscall numbers defined in syscalls.h
 * Number indicates how many arguments function requires
 */
functionWithArg syscalls[] = {
    [SYS_fork] {0, sys_fork},
    [SYS_test] {1, sys_test},
	[SYS_getdents] {3, sys_getdents}
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
void syscall_handler(void) {

    uint64_t num, ret;
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

    // default return
    ret = 0;

    switch (callFunc.count) {
        case 0:
            ret = callFunc.func();
            break;
        case 1:
            ret = callFunc.func(arg0);
            break;
        case 2:
            ret = callFunc.func(arg0, arg1);
            break;
        case 3:
            ret = callFunc.func(arg0, arg1, arg2);
            break;
        case 4:
            ret = callFunc.func(arg0, arg1, arg2, arg3);
            break;
        case 5:
            ret = callFunc.func(arg0, arg1, arg2, arg3, arg4);
            break;
    }

    // store return value into rax register
    __asm__ __volatile__(
        "movq %0, %%rax;"
         ::"r" (ret)
    );
}

/**
 * Unused, calling this messes up rax defeating the purpose
 */
uint64_t get_sys_return() {
    uint64_t ret;
    __asm__ __volatile__(
        "movq %%rax, %0;"
         :"=r" (ret)
         :: "%rax"
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
