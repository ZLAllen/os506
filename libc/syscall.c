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

    __asm__ __volatile__
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
    __asm__ __volatile__
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

	__asm__ __volatile__
		("int $0x80"
		:"=a" (ret)
    	: "0"(SYS_open), "D"(file), "S"(flags)
		: "cc", "rcx", "r11"
	);

	return ret;

}


int close(unsigned int fd){

   int ret;

   __asm__ __volatile__
	   ("int $0x80"
        : "=a"(ret)
        : "0"(SYS_close), "D"(fd)
		: "cc", "rcx", "r11"
       );
   return ret;
}

int chdir(const char* path){
    int ret;
    __asm__ __volatile__ 
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

    __asm__ __volatile__("int $0x80"
         :"=a"(ret)
         :"0"(SYS_pipe), "D"(pipefd)
		 :"cc", "rcx", "r11"
        );
    return ret;

}

int dup2(int srcfd, int destfd) {

    int ret;
    
    __asm__ __volatile__
        ("int $0x80"
         :"=a"(ret)
         :"0"(SYS_dup2), "D"(srcfd), "S"(destfd)
         :"cc", "rcx", "r11"
        );
    return ret;

}


int execvpe(char *path, char *argv[], char *envp[]){

    int ret; 

    __asm__ __volatile__("int $0x80"
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
    __asm__ __volatile__
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

	__asm__ __volatile__
		("int $0x80"
			:"=a"(ret)
			:"0"(SYS_getdents), "D"(fd), "S"(d), "d"(count)
			:"cc", "rcx", "r11", "memory"
			);

	return ret;
}



void *brk(void* addr){

    void *ret;

    __asm__ __volatile__("int $0x80"
         :"=a"(ret)
         :"0"(SYS_brk), "D"(addr)
		 :"cc", "rcx", "r11", "memory"
        );

    return ret;

}

int dup(int fd){
    int ret;

    __asm__ volatile
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

    __asm__ volatile
        ("int $0x80"
         :"=a"(ret)
         :"0"(SYS_wait4), "D"(upid), "S"(status)
         :"cc", "rcx", "r11"
        );
    
    return ret;
}


char *getcwd(char *buf, size_t size){
    char* ret;

    __asm__ volatile
        ("int $0x80"
         :"=a"(ret)
         :"0"(SYS_getcwd), "D"(buf), "S"(size)
         :"cc", "rcx", "r11", "memory"
        );

    return ret;
}

void yield() {

	int ret;

    __asm__ volatile
        ("int $0x80"
         :"=a"(ret)
         :"0"(SYS_yield)
         :"cc", "rcx", "r11", "memory"
        );
}

int ps(char *buf) {

    uint64_t num = SYS_ps;
    int ret;

    __asm__ volatile ("int $0x80"
            :"=a"(ret)
            :"0"(num), "D"(buf)
            :"cc", "rcx", "r11", "memory"
            );

   return ret; 
}

uint64_t test(uint64_t arg) {

    uint64_t num = SYS_test;
    int ret;

    __asm__ volatile ("int $0x80"
            :"=a"(ret)
            :"0"(num), "D"(arg)
            :"cc", "rcx", "r11", "memory"
            );

   return ret; 
}

pid_t fork() {
    uint64_t num = SYS_fork;
    uint64_t addr = 0;
    uint64_t rsp = 0;
    pid_t ret;


    __asm__ volatile ("movq 0(%%rsp), %0;" :"=r"(addr));
    __asm__ volatile ("movq %%rsp, %0;" :"=r"(rsp));

    rsp += 8;

    
    __asm__ __volatile__("int $0x80"
         :"=a"(ret)
         :"0"(num), "D"(addr), "S"(rsp)
		 :"cc", "rcx", "r11", "memory"
        );

    return ret;
}

void exit() {
	int ret;
    __asm__ __volatile__
        ("int $0x80"
         :"=a"(ret)
         :"0"(SYS_exit)
         :"cc", "rcx", "r11", "memory"
        );
}

