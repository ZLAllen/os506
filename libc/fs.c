#include <sys/dirent.h>
#include <stdio.h>
#include <sys/fs.h>

/*
opens and returns a directory stream corresponding to the directory name. NULL on error
*/
void *opendir(const char *name)
{
	//perform checks
    if(!dirname)
    	return NULL;

    struct dstream *dirp;
    int fd = open(name, O_RDONLY | O_DIRECTORY);

    if(fd < 0 && fd >= MAX_FD)
    	return NULL;
               
    dirp = malloc(sizeof(*dirp));//replace with brk
    if(!dirp)
    	return NULL;

    dirp->fd = fd;
	dirp->offset = 0;
    
	return dirp;

}


/*
returns a pointer to a dirent structure representing the next directory entry
in the directory stream pointed to by dirp. NULL on EOD/ERROR
*/
struct linux_dirent *readdir(struct dstream *dirp)
{
	if(!dirp)
    	return NULL;

    int size = getdents((unsigned int) dirp->fd, (struct linux_dirent*) dirp->buff, (unsigned int) sizeof(dipr->buff));
	if size <= 0
		return NULL;
	dirp->offset = 0;
	dirp->size = (size_t)size
	struct linux_dirent *drent =  (struct linux_dirent*)(dstream->buff + dstream->offset);
	dstream->offset += dirp->d_reclen;

    return drent;

}


/*
1. closes the directory stream associated with dirp
2. closes the underlying file descriptor associated with dirp. -1 on ERROR
*/
int closedir(struct dstream *dirp)
{
	if(!dirp)
    	return -1;
    
    int fd = dirp->fd;
    free(dirp);	
    int ret =close(fd);
    return ret;

}

