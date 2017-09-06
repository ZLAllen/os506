
#include <fcntl.h>

void* brk(void* addr);
int getdents(int fd, struct linux_dirent* d, int count);
