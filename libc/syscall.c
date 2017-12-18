#include <sysdefs.h>
#include <syscall.h>
#include <stdlib.h>
#include <dirent.h>

/**
 * Kernel syscall utility functions
 * so making syscalls aren't so tedious
 */
ssize_t read(unsigned int fd, char* buf, size_t size){
    int ret;

    __asm
        (
         "int $0x80"
         : "=a" (ret)
         : "0"(SYS_read), "D"(fd), "S"(buf), "d"(size)
         : "cc", "rcx", "r11", "memory"
        );
    return ret;
}

ssize_t write(unsigned int fd, const char* buf, size_t size){
    int ret;
    __asm
        (
         "int $0x80"
         : "=a" (ret)
         : "0"(SYS_write), "D"(fd), "S"(buf), "d"(size)
         : "cc", "rcx", "r11", "memory"
        );
    return ret;
}


int open(const char *file, int flags) {

	int ret;

	__asm
		("int $0x80"
		:"=a" (ret)
    	: "0"(SYS_open), "D"(file), "S"(flags)
		: "cc", "rcx", "r11"
	);

	return ret;

}


int close(unsigned int fd){

   int ret;

   __asm
	   ("int $0x80"
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
         "int $0x80"
         : "=a" (ret)
         : "0"(SYS_chdir), "D"(path)
         : "cc", "rcx", "r11"
        );

    return ret;
}

int pipe(int pipefd[]) {

    int ret;

    __asm("int $0x80"
         :"=a"(ret)
         :"0"(SYS_pipe), "D"(pipefd)
		 :"cc", "rcx", "r11"
        );
    return ret;

}

int dup2(int srcfd, int destfd) {

    int ret;
    
    __asm
        ("int $0x80"
         :"=a"(ret)
         :"0"(SYS_dup2), "D"(srcfd), "S"(destfd)
         :"cc", "rcx", "r11"
        );
    return ret;

}


int execvpe(char *path, char *argv[], char *envp[]){

    int ret; 

    __asm("int $0x80"
			:"=a" (ret)
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

	int ret;

	__asm
		("int $0x80"
			:"=a"(ret)
			:"0"(SYS_getdents), "D"(fd), "S"(d), "d"(count)
			:"cc", "rcx", "r11", "memory"
			);

	return ret;
}



void *brk(void* addr){

    void *ret;

    __asm("int $0x80"
         :"=a"(ret)
         :"0"(SYS_brk), "D"(addr)
		 :"cc", "rcx", "r11", "memory"
        );

    return ret;

}

int dup(int fd){
    int ret;

    __asm
        ("int $0x80"
         :"=a"(ret)
         :"0"(SYS_dup), "D"(fd)
         :"cc", "rcx", "r11"
        );

    return ret;
}

pid_t wait(int *status) {
    return waitpid(-1, status);
}

pid_t waitpid(pid_t upid, int* status){
    int ret;

    __asm
        ("int $0x80"
         :"=a"(ret)
         :"0"(SYS_wait4), "D"(upid), "S"(status)
         :"cc", "rcx", "r11"
        );
    
    return ret;
}


char *getcwd(char *buf, size_t size){
    char* ret;

    __asm
        ("int $0x80"
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
    uint64_t addr = 0;
    pid_t ret;


    __asm__ volatile ("movq 0(%%rsp), %0;" :"=r"(addr));

    
    __asm("int $0x80"
         :"=a"(ret)
         :"0"(num), "D"(addr)
		 :"cc", "rcx", "r11", "memory"
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

