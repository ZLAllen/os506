#include <sys/defs.h>
#include <fcntl.h>


void* brk(void* addr);
int wait4(pid_t upid, int* stat_addr, int options);
int dup(int fd);
int getdents(int fd, struct linux_dirent* dirent, int count);
