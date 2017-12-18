#include <stdio.h>
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
   
    struct dstream *dirp;
    int fd = open(name, O_DIRECTORY | O_RDONLY);//directory and read only flags

	if(fd < 0)
	{
		printf("error in syscall open\n");
		return NULL;
	}

    dirp = malloc(sizeof(*dirp));
	if(!dirp)
	{
		printf("dirp cannot be allocated\n");
		return NULL;
	}

	dirp->size = dirp->offset = 0;//will set selective fields in readdir
    dirp->fd = fd;
	
    return dirp;
}



//TODO this code can be made better
struct linux_dirent *readdir(struct dstream *open_dirp)
{
    if(!open_dirp)
        return NULL;

	struct dstream *dirp = open_dirp;	
	int  size;

	if(dirp->size <= dirp->offset) 
	{

    	size = getdents((unsigned int) dirp->fd, (struct linux_dirent*) dirp->buff, (unsigned int) sizeof(dirp->buff));//bytes read on success
    	
		if (size <= 0) 
        	return NULL;

		dirp->size = (size_t)size;
    	dirp->offset = 0; 
	}

    struct linux_dirent *drent =  (struct linux_dirent*)(dirp->buff + dirp->offset);//buff holds the dirent
    
	dirp->offset += drent->d_reclen;//incr by length of this dirent

    return drent;

}


int closedir(struct dstream *dirp)
{

    if(!dirp)
        return -1;

    int fd = dirp->fd;
    free(dirp);	
    int ret =close(fd);
    return ret;

}


