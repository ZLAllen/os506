#include <sys/defs.h>
#include <sysdefs.h>
#include <dirent.h>
#include <sys/syscall_handler.h>

/**
 * Syscall utility functions
 */

ssize_t read(unsigned int fd, char *buf, size_t size);
ssize_t write(unsigned int fd, const char *buf, size_t size);
int open(const char *file, int flag);
int close(unsigned int fd);

int chdir(const char *path);
int pipe(int pipefd[]);
int dup2(int srcfd, int destfd);
int execvpe(char *path, char *argv[], char *envp[]);
void *brk(void *addr);
int dup(int fd);
int wait(int *status);
int wait4(pid_t upid, int *status, int options);
char *getcwd(char *buf, size_t size);

void yield();
uint64_t test(uint64_t arg);
uint64_t test3(uint64_t arg1, uint64_t arg2, uint64_t arg3);
void sleep(uint64_t ms);
pid_t fork();
void exit();


int getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count);

// utility functions to set arguments for syscall
void syscallArg0(uint64_t num);
void syscallArg1(uint64_t num, uint64_t arg0);
void syscallArg2(uint64_t num, uint64_t arg0, uint64_t arg1);
void syscallArg3(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2);
void syscallArg4(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3);
void syscallArg5(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);


