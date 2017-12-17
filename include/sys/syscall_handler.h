#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <sys/defs.h>
#include <dirent.h>
#include <sysdefs.h>

#define GET_SYS_RET(sysReturn) \
    __asm__ volatile("movq %%rax, %0;":"=r" (sysReturn));

// syscall functions
int64_t sys_test(uint64_t testArg);
int64_t sys_sleep(uint64_t ms);
int64_t sys_fork(uint64_t addr);
int64_t sys_exit();
int64_t sys_yield();
int64_t sys_test3(uint64_t arg1, uint64_t arg2, uint64_t arg3);

int64_t sys_getdents(unsigned int fd, struct linux_dirent* dirp, unsigned int count);//convert ars to uint64_t inside code
int64_t sys_open(char *name, int flags);
int64_t sys_brk(void *addr);


int64_t sys_execve(char *file, char *argv[], char *envp[]);

// syscall handler
void syscall_handler(void);
uint64_t get_sys_return();

#endif
