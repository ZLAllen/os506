#ifndef FS_H
#define FS_H

#include <sys/defs.h>

struct file 
{
    struct file_ops *f_op; /* file operations table */
    unsigned long f_count; /* file object's usage count */
    int f_flags;           /* flags specified on open */
    off_t f_pos;           /* file offset (file pointer) */
    void *private_data;    /* tty driver hook, for TARFS it points to file's ustar header */
    unsigned long f_size;
    struct file* free;

    char name[256];
    uint64_t start, end, curr;
    struct file *child[100];
    uint64_t curr_child;
    uint64_t type;
    uint64_t inum;
};

struct file_ops 
{ 
    struct file* (*open) (const char *);
    ssize_t (*read) (struct file *, char *, size_t, off_t *);
    int (*close) (struct file *);
};


#endif


