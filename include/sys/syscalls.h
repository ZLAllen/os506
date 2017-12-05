//got error so adding these lines
#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <sys/defs.h>
#include <sys/dirent.h>

// syscall numbers
#define SYS_fork 57
#define SYS_test 50

// syscall functions
int sys_getdents(unsigned int fd, struct linux_dirent* dirp, unsigned int count);
uint64_t sys_test(uint64_t testArg);
uint64_t sys_fork();

// syscall handler
void syscall(void);
uint64_t get_sys_return();

// utility functions to set arguments for syscall
void syscallArg0(uint64_t arg0);
void syscallArg1(uint64_t num, uint64_t arg0);
void syscallArg2(uint64_t num, uint64_t arg0, uint64_t arg1);
void syscallArg3(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2);
void syscallArg4(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3);
void syscallArg5(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);

#endif
