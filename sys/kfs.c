#include <sys/kfs.h>
#include <sys/schedule.h>
#include <sys/tarfs.h>

/*
 *syscall handler forwards sys calls to these functions
 * */



int sysopen(char *name, int flags)
{

	if(!name)
		return -1;

	int fd = get_free_fd(); 

	struct file *filep = tfs_open(name, flags);//??check path
	current->fdarr[fd] = filep;//add it to list of oen files

	return fd;
}


int sysclose(int fd)
{

	struct file *filep = current->fdarr[fd];
	if (!filep)
		return -1;

	int ret = filep->fop->close(filep);
	current->fdarr[fd] = NULL;

	return ret;
}


uint64_t sysgetdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count) 
{

	
	if (!dirp || count <= 0)
        return -1; 

    if(fd <0 || fd >= MAX_FD)
        return -1;

    struct file *filep = current->fdarr[fd];//file object pointed by the fd

    if(!filep)
        return -1;

    return (uint64_t) filep->fop->readdir(filep, dirp, count);//num bytes read

}
