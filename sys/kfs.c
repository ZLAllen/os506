#include <sys/kprintf.h>
#include <sys/dirent.h>
#include <sys/schedule.h>

int getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count) 
{
    if(fd <0 || fd >= MAX_FD)
        return -1;

    struct file *filep = current->fdarr[fd];
    if(!filep)
        return -1;

    return filep->fop->readdir(filep, dirp, count);
}






