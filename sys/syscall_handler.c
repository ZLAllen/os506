#include <sys/defs.h>
#include <sys/syscall_handler.h>
#include <sys/schedule.h>
#include <sys/kprintf.h>
#include <dirent.h>
#include <sys/ktime.h>

extern uint64_t ms;

/**
 * Syscalls definitions
 * These functions should not be directly called. Use sys/syscall.c instead.
 */

/** current process (sys/schedule.c) */
extern task_struct *current;

uint64_t sys_yield() {
    run_next_task();
    return 0;
}

uint64_t sys_test(uint64_t testArg) {
    __asm__ __volatile__(PUSHREGS);
    kprintf("print me. Argument is %d\n", testArg);
    __asm__ __volatile__(POPREGS);
    return 9001;
}

/**
 * Sleep process for given number of milliseconds
 */
uint64_t sys_sleep(uint64_t msec) {
    current->sleep_time = ms + msec;
    return 0;
}


/**
 * Fork current process
 * Creates new process as a child of the current
 *
 * Do NOT use this directly. Use fork() in syscall.h!
 */
uint64_t sys_fork() {

    uint64_t parent_rip;

    // create child process
    task_struct *child = fork_process(current);

    // get current process RIP based on stack
    // assumes fork() from syscall.h was called
    __asm__ __volatile__("mov 160(%%rsp), %0":"=r"(parent_rip));

    // schedule new process like any other
    schedule(child, parent_rip);

    // return value of child should be 0
    // parent would return pid
    child->rax = 0;

    // return child PID to the parent
    return child->pid;
}

/**
 * Exit current process
 * Sets current process to not runnable
 * Process will be removed from available_tasks
 */
uint64_t sys_exit() {
    current->runnable = false;
    return 0;
}


uint64_t sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count) 
{
    if (!dirp || count <= 0)
        return -1; 

    if(fd <0 || fd >= MAX_FD)
        return -1;

    struct file *filep = current->fdarr[fd];//file object pointed by the fd

    if(!filep)
        return -1;

    return (uint64_t) filep->fop->readdir(filep, dirp, count);//num bytes read

}


/**
 * Supported syscalls
 * Functions defined above
 * Syscall numbers defined in syscalls.h
 * Number indicates how many arguments function requires
 */
functionWithArg syscalls[] = {
    [SYS_yield] {0, sys_yield}, // 24
    [SYS_sleep] {1, sys_sleep}, // 35
    [SYS_fork] {0, sys_fork}, // 57
    [SYS_test] {1, sys_test}, // 50
    [SYS_exit] {0, sys_exit}, // 60
    [SYS_getdents] {3, sys_getdents} // 78
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
