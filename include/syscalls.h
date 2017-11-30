//got error so adding these lines
#ifndef _SYSCALLS_H
#define _SYSCALLS_H


#include <sys/defs.h>
#include <fcntl.h>

// syscall numbers
#define SYS_test 50

void* brk(void* addr);
int wait4(pid_t upid, int* stat_addr, int options);
int dup(int fd);
void sys_test(void);

void syscall(void);

#endif
