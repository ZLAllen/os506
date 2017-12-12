#include <sys/kprintf.h>
#include <sys/dirent.h>
#include <sys/schedule.h>


//on error -1 else num bytes
int getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count) 
{
    if(fd <0 || fd >= MAX_FD)
        return -1;

    struct file *filep = current->fdarr[fd];
    if(!filep)
        return -1;
	//call the readdir function 
    return filep->fop->readdir(filep, dirp, count);
}






