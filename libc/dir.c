#include <stdio.h>
#include <syscall.h>
#include <syscall.h>
#include <stdlib.h>
#include <fcntl.h>

/*
   opens and returns a directory stream corresponding to the directory name. NULL on error
   */
struct dstream *opendir(const char *name)
{
    //perform checks
    if(!name)
        return NULL;

    //while(1);
    struct dstream *dirp;
    int fd = open(name, O_DIRECTORY | O_RDONLY);//directory and read only flags

    if(fd < 0)//fd boundary check; doing MAX check elsewhere
        return NULL;

    //while(1);	
    dirp = malloc(sizeof(*dirp));
    if(!dirp)
        return NULL;

    dirp->size = 0;
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

    int size = getdents((unsigned int) dirp->fd, (struct linux_dirent*) dirp->buff, (unsigned int) sizeof(dirp->buff));

    if (size <= 0) 
        return NULL;


    dirp->offset = 0;
    dirp->size = (size_t)size;

    struct linux_dirent *drent =  (struct linux_dirent*)(dirp->buff + dirp->offset);//read dirent at offset
    dirp->offset += drent->d_reclen;//increment the ptr

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

