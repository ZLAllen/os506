#include <syscall.h>

/**
 * Kernel syscall utility functions
 * so making syscalls aren't so tedious
 */

void yield() {
    uint64_t num = SYS_yield;

    syscallArg0(num);

    __asm__ volatile ("int $0x80"
            ::: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 
}

void sleep(uint64_t ms) {

    uint64_t num = SYS_sleep;
    uint64_t ret;
    syscallArg1(num, ms);

    __asm__ volatile ("int $0x80"
            :"=r" (ret)
            :: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 

    yield();
}

uint64_t test(uint64_t arg) {

    uint64_t num = SYS_test;
    uint64_t ret;
    syscallArg1(num, arg);

    __asm__ volatile ("int $0x80"
            :"=r" (ret)
            :: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 

    return ret;
}

uint64_t test3(uint64_t arg1, uint64_t arg2, uint64_t arg3) {

    uint64_t num = SYS_test3;
    uint64_t ret;
    syscallArg3(num, arg1, arg2, arg3);

    __asm__ volatile ("int $0x80"
            :"=r" (ret)
            :: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 

    return ret;
}

pid_t fork() {
    uint64_t num = SYS_fork;
    pid_t ret;

    syscallArg0(num);

    __asm__ volatile ("int $0x80"
            :"=r" (ret)
            :: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 
    return ret;
}

void exit() {
    uint64_t num = SYS_exit;

    syscallArg0(num);

    __asm__ volatile ("int $0x80"
            ::: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 

    yield();
}


int pipe(int pipefd[]) {
	
	uint64_t num = SYS_pipe;
	int ret;

	syscallArg1(num, (uint64_t)pipefd);

	__asm__ volatile("int $0x80"
			:"=r"(ret)
			:: "%r8", "%rcx", "%rdx", "%rsi", "%rdi"
			);

	return ret;
}


int close(unsigned int fd){

    uint64_t num = SYS_close;
	int ret;

    syscallArg1(num, (uint64_t) fd);

    __asm__ volatile("int $0x80"
            : "=r"(ret)
            :: "%r8", "%rcx", "%rdx", "%rsi", "%rdi"
            );
    return ret;
}

void *brk(void* addr){

    int num = SYS_brk;
    void* ret;

    syscallArg1(num, (uint64_t)addr);

    __asm volatile("int $0x80"
            :"=r"(ret)
            ::"%r8", "%rcx", "%rdx", "%rsi", "%rdi"
            );

    return ret;
}


int getdents(unsigned int fd, struct linux_dirent *d, unsigned int count){

    uint64_t num = SYS_getdents;
    int ret;

    syscallArg3(num, (uint64_t)fd,(uint64_t)d, (uint64_t)count);

    __asm volatile("int $0x80"
            :"=r"(ret)
            ::"%r8", "%rcx", "%rdx", "%rsi", "%rdi" 
            );

    return ret;
}

int open(const char *file, int flags) {

    uint64_t num = SYS_open;
    int ret;

    syscallArg2(num, (uint64_t)file, (uint64_t)flags);

    __asm__ volatile ("int $0x80"
            :"=r" (ret)
            :: "%r8", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 

    return ret;
}

ssize_t read(unsigned int fd, char* buf, size_t size){


    uint64_t num = SYS_read;
    uint64_t ret;

    while(1);
    syscallArg3(num, (uint64_t)fd, (uint64_t)buf, (uint64_t)size);

    __asm__ volatile ("int $0x80"
            :"=r" (ret)
            :: "%r8", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 

    return ret;
}

ssize_t write(unsigned int fd, const char* buf, size_t size){


    uint64_t num = SYS_write;
    uint64_t ret;

    syscallArg3(num, (uint64_t)fd, (uint64_t)buf, (uint64_t)size);

    __asm__ volatile ("int $0x80"
            :"=r" (ret)
            :: "%r8", "%rcx", "%rdx", "%rsi", "%rdi"
            ); 

    return ret;
}

int execve(char *path, char **argv, char **envp){

	uint64_t num = SYS_execve;
	int ret;

	syscallArg3(num, (uint64_t)path, (uint64_t)argv, (uint64_t)envp);

	__asm__ volatile ("int $0x80"
			:"=r" (ret)
			:: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
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
        ("movq %0, %%rdi" ::"r" (arg0));
}

void syscallArg2(uint64_t num, uint64_t arg0, uint64_t arg1) {
    __asm__ __volatile__
        ("movq %0, %%rax" :: "r" (num));
    __asm__ __volatile__
        ("movq %0, %%rdi;" 
         "movq %1, %%rsi;"
         ::"r" (arg0), "r" (arg1)
        );
}

void syscallArg3(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
    __asm__ __volatile__
        ("movq %0, %%rax" :: "r" (num));
    __asm__ __volatile__
        ("movq %0, %%rdi;" 
         "movq %1, %%rsi;"
         "movq %2, %%rdx;"
         ::"r" (arg0), "r" (arg1), "r" (arg2)
         : "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
         
        );
}

void syscallArg4(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
}
void syscallArg5(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4) {
}
