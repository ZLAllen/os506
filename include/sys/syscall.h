#include <sys/defs.h>
#include <sys/syscalls.h>

/**
 * Kernel syscall utility functions
 */

void yield();
uint64_t test(uint64_t arg);
uint64_t fork();
void exit();

// utility functions to set arguments for syscall
void syscallArg0(uint64_t num);
void syscallArg1(uint64_t num, uint64_t arg0);
void syscallArg2(uint64_t num, uint64_t arg0, uint64_t arg1);
void syscallArg3(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2);
void syscallArg4(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3);
void syscallArg5(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);

