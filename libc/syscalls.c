#include <sysdefs.h>
#include <syscalls.h>
#include <stdlib.h>
#include <fcntl.h>

// in x86-64, we can free rcx and r11 for kernel

int read(int fd, char* buf, int size){
    int ret;

    __asm
        (
         "syscall"
         : "=a" (ret)
         : "0"(__NR_read), "D"(fd), "S"(buf), "d"(size)
         : "cc", "rcx", "r11", "memory"
        );
    return ret;
}

int write(int fd, const char* buf, int size){
    int ret;
    __asm
        (
         "syscall"
         : "=a" (ret)
         : "0"(__NR_write), "D"(fd), "S"(buf), "d"(size)
         : "cc", "rcx", "r11", "memory"
        );
    return ret;
}

int open(const char* file, int flag){
    int ret;
    __asm 
        (
         "syscall"
         : "=a" (ret)
         : "0"(__NR_open), "D"(file),"S"(flag)
         : "cc", "rcx", "r11"
        );
    return ret;
}

int close(int fd){
   int ret;

   __asm
       (
        "syscall"
        : "=a"(ret)
        : "0"(__NR_close), "D"(fd)
        : "cc", "rcx", "r11"
       );
   return ret;
}

int fork(){
    int ret;

    __asm 
        (
         "syscall"
         : "=a"(ret)
         : "0"(__NR_fork)
         : "cc", "rcx", "r11"
        );
    return ret;
}

void exit(int status){
    __asm 
        (
         "syscall"
         :
         : "a"(__NR_exit), "D"(status)
         : "cc"
        );
}

int chdir(char* path){
    int ret;
    __asm 
        (
         "syscall"
         : "=a" (ret)
         : "0"(__NR_chdir), "D"(path)
         : "cc", "rcx", "r11"
        );

    return ret;
}

int pipe(int pipefd[]) {
    int ret;

    __asm
        ("syscall"
         :"=a"(ret)
         :"0"(__NR_pipe), "D"(pipefd)
         :"cc", "rcx", "r11"
        );
    return ret;

}

int dup2(int srcfd, int destfd) {

    int ret;
    
    __asm
        ("syscall"
         :"=a"(ret)
         :"0"(__NR_dup2), "D"(srcfd), "S"(destfd)
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
         :"0"(__NR_execve), "D"(path), "S"(argv), "d"(envp)
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
         :"0" (__NR_fstat), "D"(fd), "S"(buf)
         :"cc", "rcx", "r11"
        );
    return ret;
}
*/

int getdents(int fd, struct linux_dirent *d, int count){

    __asm("syscall"
            :"=a"(ret)
            :"0"(__NR_getdents), "D"(fd), "S"(d), "d"(count)
            :"cc", "rcx", "r11", "memory"
         );
    
    return ret;
}
void* brk(void* addr){
    void* ret;

    __asm
        ("syscall"
         :"=a"(ret)
         :"0"(__NR_brk), "D"(addr)
         :"cc", "rcx", "r11", "memory"
        );
    return ret;

}

int dup(int fd){
    int ret;

    __asm
        ("syscall",
         :"=a"(ret)
         :"0"(__NR_dup), "D"(fd)
         :"cc", "rcx", "r11"
        );

    return ret;
}

int pipe(int fd[2]){
    int ret;

    __asm
        ("syscall",
         :"=a"(ret)
         :"0"(__NR_pipe), "D"(fd)
         :"cc", "rcx", "r11"
        );

    return ret;

}

int wait4(pid_t upid, int* stat_addr, int options){
    // we ignore struct rusage for now
    int ret;

    __asm
        ("syscall"
         :"=a"(ret)
         :"0"(__NR_wait4), "D"(upid), "s"(stat_addr), "d"(options)
         :"cc", "rcx", "r11"
        );
    
    return ret;
}

