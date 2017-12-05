#include <sys/dirent.h>
#include <stdio.h>


//dstream struct when correct else NULL
struct dstream *opendir(const char *path)
{
    struct dstream *dirp = (struct dstream *) malloc(sizeof(struct dstream));
    dirp = (struct dstream*) syscallArg2(SYS_opendir, (uint64_t)path, (uint64_t)dirp); 
    if(!dirp->node) 
		free(dirp)
    return dirp;
} 

//dirent struct when correct else NULL
struct dirent* readdir(struct dstream *dirp)
{   
    struct dirent* drent = (struct dirent*) syscallArg1(SYS_readdir, (uint64_t)dirp);
    return drent;
}

//0 when correct else -1 on error
int closedir(struct dstream *dirp)
{
    int val = (int) sysallArg1(SYS_closedir, (uint64_t)dirp);
    if (val == -1)
        free(node);

    return val;
}


/*
opendir, readdir, closedir
*/

/*
opens and returns a directory stream corresponding to the directory name. NULL on error

void *opendir(const char *dirname)
{
        //perform checks
        if(!dirname)
                return NULL;

        struct dstream *dirp;

        // fix this
        int fd = 0;
        if(fd < 0)
        {
                printf("fd is < 0");
                return NULL;
        }
        dirp = malloc();
        if(!dirp)
        {
                printf("could not allocate memory for dirp");
                return NULL;

        }
        //check this
        dirp->fd = fd;
	dirp->size = 0;
	dirp->offset = 0;
        return dirp;

}



returns a pointer to a dirent structure representing the next directory entry
in the directory stream pointed to by dirp. NULL on EOD/ERROR

struct dirent *readdir(struct dstream *dirp)
{

        if(!dirp)
        {
                printf("dirp is NULL");
                return NULL;
        }

        struct dirent *drent = NULL;//how to get dirent from dstream?
	drent->fd = 0;
	drent->size = 0;
	drent->offset = 0;
        return drent;

}



1. closes the directory stream associated with dirp
2. closes the underlying file descriptor associated with dirp. -1 on ERROR

int closedir(struct dstream *dirp)
{
        if(!dirp)
        {
                printf("dirp is NULL");
                return -1;
        }
        int fd = dirp->fd;
        free(dirp);	
        int ret = 0 //close(fd);
        return ret;

}
*/
