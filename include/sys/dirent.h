#ifndef _DIRENT_H
#define _DIRENT_H

#include <sys/fs.h>

enum file_type {DIR, FILE};


struct dirent 
{
    uint64_t inum;       /* inode number */
    //off_t d_off;       /* offset to the next dirent */
    //unsigned short d_reclen;    /* length of this record */
    //unsigned char d_type;      /* type of file; not supported by all file system types */
    char name[256]; /* filename */
};


//directory stream
struct dstream
{
        /*int fd;
        size_t size;
        size_t offset;
	char data[100]; //size?*/
	struct file *node;
    uint64_t curr;
    struct dirent drent;
	
};


struct f_desc 
{
    struct file *node;
    uint64_t curr;
    uint64_t perm;
    uint64_t inum;
};

struct dstream *opendir(const char *dirname);
struct dirent *readdir(struct dstream *dirp);
int closedir(struct dstream *dirp);



struct linux_dirent {
    unsigned long d_ino;
    unsigned long d_off;
    unsigned short d_reclen; 
    unsigned char d_type;
    struct linux_dirent* free;
    char d_name[];
};








#endif
