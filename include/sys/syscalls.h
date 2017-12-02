//got error so adding these lines
#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <sys/defs.h>

// syscall numbers
#define SYS_test 50

// syscall functions
void sys_test(uint64_t testArg);

// syscall handler
void syscall(void);

// utility functions to set arguments for syscall
void syscallArg0(uint64_t arg0);
void syscallArg1(uint64_t num, uint64_t arg0);
void syscallArg2(uint64_t num, uint64_t arg0, uint64_t arg1);
void syscallArg3(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2);
void syscallArg4(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3);
void syscallArg5(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);

#endif
