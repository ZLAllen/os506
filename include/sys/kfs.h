#ifndef _KFS_H
#define _KFS_H

#include <dirent.h>

int sysopen(char *name, int flags);

int sysclose(int fd);

uint64_t sysgetdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count);


#endif
