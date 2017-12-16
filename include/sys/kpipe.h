#ifndef _KPIPE_H
#define _KPIPE_H

#include <sys/defs.h>
#include <sys/files.h>

ssize_t rhead_read(struct file* filep, char* buf, size_t size, off_t* offset);
ssize_t rhead_write(struct file *fp, const char *buf, size_t count, off_t *offset);


ssize_t whead_read(struct file *fp, char *buf, size_t count, off_t *offset);
ssize_t whead_write(struct file* filep, char* buf, size_t size, off_t* offset);

int pipe_close(struct file* file);
int pipe_readdir(struct file*, void*, unsigned int); // return error 

#endif
