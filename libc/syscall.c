#include <sysdefs.h>
#include <syscall.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

/**
 * Kernel syscall utility functions
 * so making syscalls aren't so tedious
 */
ssize_t read(int fd, void* buf, size_t size){
    int ret;

    __asm
        (
         "syscall"
         : "=a" (ret)
         : "0"(SYS_read), "D"(fd), "S"(buf), "d"(size)
         : "cc", "rcx", "r11", "memory"
        );
    return ret;
}

ssize_t write(int fd, const void* buf, size_t size){
    int ret;
    __asm
        (
         "syscall"
         : "=a" (ret)
         : "0"(SYS_write), "D"(fd), "S"(buf), "d"(size)
         : "cc", "rcx", "r11", "memory"
        );
    return ret;
}


int open(const char *file, int flags) {

	uint64_t num = SYS_open;
	uint64_t ret;

	syscallArg2(num, (uint64_t)file, (uint64_t)flags);

	__asm__ volatile ("int $0x80"
		:"=r" (ret)
		:: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
	);

	return ret;

}


int close(int fd){
   int ret;

   __asm
       (
        "syscall"
        : "=a"(ret)
        : "0"(SYS_close), "D"(fd)
        : "cc", "rcx", "r11"
       );
   return ret;
}

int chdir(const char* path){
    int ret;
    __asm 
        (
         "syscall"
         : "=a" (ret)
         : "0"(SYS_chdir), "D"(path)
         : "cc", "rcx", "r11"
        );

    return ret;
}

int pipe(int pipefd[]) {
    int ret;

    __asm
        ("syscall"
         :"=a"(ret)
         :"0"(SYS_pipe), "D"(pipefd)
         :"cc", "rcx", "r11"
        );
    return ret;

}

int dup2(int srcfd, int destfd) {

    int ret;
    
    __asm
        ("syscall"
         :"=a"(ret)
         :"0"(SYS_dup2), "D"(srcfd), "S"(destfd)
         :"cc", "rcx", "r11"
        );
    return ret;

}

// TODO test if memory should be there
int execve(char *path, char *argv[], char *envp[]){
    int ret;
    
    __asm
        ("syscall"
         :"=a"(ret)
         :"0"(SYS_execve), "D"(path), "S"(argv), "d"(envp)
         :"cc", "rcx", "r11", "memory"
        );
    return ret;

}
// come back later
/*
int fstat(int fd, struct stat *buf){
    int ret;
    __asm
        ("syscall"
         :"=a" (ret)
         :"0" (SYS_fstat), "D"(fd), "S"(buf)
         :"cc", "rcx", "r11"
        );
    return ret;
}
*/


int getdents(unsigned int fd, struct linux_dirent *d, unsigned int count){

	uint64_t num = SYS_getdents;
	int ret;

	syscallArg3(num, (uint64_t)fd,(uint64_t)d, (uint64_t)count);

	__asm volatile("int $0x80"
			:"=r"(ret)
			::"%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
			);

	return ret;
}



void* brk(void* addr){

	int num = SYS_brk;
    void* ret;

	syscallArg1(num, (uint64_t)addr);

    __asm volatile("int $0x80"
         :"=r"(ret)
         ::"%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
        );

    return ret;

}

int dup(int fd){
    int ret;

    __asm
        ("syscall"
         :"=a"(ret)
         :"0"(SYS_dup), "D"(fd)
         :"cc", "rcx", "r11"
        );

    return ret;
}


int wait4(pid_t upid, int* status, int options){
    // we ignore struct rusage for now
    int ret;

    __asm
        ("syscall"
         :"=a"(ret)
         :"0"(SYS_wait4), "D"(upid), "S"(status),"d"(options)
         :"cc", "rcx", "r11"
        );
    
    return ret;
}


char *getcwd(char *buf, size_t size){
    char* ret;

    __asm
        ("syscall"
         :"=a"(ret)
         :"0"(SYS_getcwd), "D"(buf), "S"(size)
         :"cc", "rcx", "r11", "memory"
        );

    return ret;
}

void yield() {
    uint64_t num = SYS_yield;

    syscallArg0(num);

    __asm__ volatile ("int $0x80"
        ::: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
    ); 
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

