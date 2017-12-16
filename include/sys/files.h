#ifndef FS_H
#define FS_H

#include <sys/defs.h>

enum ftype {DIRECTORY, FILE};

struct file 
{
    struct file_ops *fop; 
    unsigned long count;      
    off_t* offset;           
    void *data;   
    unsigned long size;
	struct file* free;
	int flags; //this is needed to set w/r perm
};


struct file_ops 
{ 
    struct file* (*open) (const char *, int pflags);
    ssize_t (*read) (struct file *, char *, size_t, off_t*);
    ssize_t (*write) (struct file*, char*, size_t, off_t*);
    int (*close) (struct file *);
	int (*readdir) (struct file *, void *, unsigned int);
	int (*closedir) (struct file *);

};















#endif


