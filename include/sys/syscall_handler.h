//got error so adding these lines
#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <sys/defs.h>
#include <dirent.h>
#include <sysdefs.h>

#define GET_SYS_RET(sysReturn) \
    __asm__ volatile("movq %%rax, %0;":"=r" (sysReturn));

// syscall functions
uint64_t sys_test(uint64_t testArg);
uint64_t sys_sleep(uint64_t ms);
uint64_t sys_fork();
uint64_t sys_exit();
uint64_t sys_getdents(unsigned int fd, struct linux_dirent* dirp, unsigned int count);


// syscall handler
void syscall_handler(void);
uint64_t get_sys_return();

#endif
