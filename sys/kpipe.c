#include <sys/kpipe.h>
#include <sys/files.h>
#include <sys/system.h>
#include <sys/stdarg.h>
#include <sys/kmalloc.h>

#define PIPEBUF_SIZE 2048


struct pipe_buf
{
	int start;//rad head
	int end;//write head
	char full;//full indicator
	char buf[PIPEBUF_SIZE];//holds data
};

//ops for read head of pipe
struct file_ops rhead_ops = {
	.open = 0,
    .read = rhead_read,
    .close = pipe_close,
	.write = 0,
	.readdir = pipe_readdir
};

//ops for write head for pipe
struct file_ops whead_ops = {
    .open = 0,
    .write = whead_write,
    .close = pipe_close,
	.read = whead_read,
	.readdir = pipe_readdir
};


ssize_t rhead_read(struct file* filep, char* buf, size_t count, off_t* offset)
{
	if(!filep || !buf)
  	{
    	return -1; // same error as in read
  	}

	if (count == 0)
		return 0;
	
	struct pipe_buf *pipe = (struct pipe_buf *)filep->data;

	if(pipe->start == pipe->end && !pipe->full)
	{
		return -1; //empty nothing to read
	}

	ssize_t nread = 0;
	while(nread <= count)
	{
		nread ++;
		*buf++ = pipe->buf[pipe->start]; //read into the buffer
		pipe->start = (pipe->start + 1) % PIPEBUF_SIZE; //move cursor to the next pos, wrap around if needed
	}

	return nread;

}


ssize_t rhead_write(struct file *filep, const char *buf, size_t count, off_t *offset) 
{
    return 0;
}

int pipe_close(struct file* filep)
{
	filep->count--;
	
	if(filep->count ==0)
	{
		kfree(filep);
	}

	return 0;
}


ssize_t whead_read(struct file *filep, char *buf, size_t count, off_t *offset) 
{
    return -1;
}

ssize_t whead_write(struct file* filep, char* buf, size_t count, off_t* offset)
{
	if(!filep || !buf)
  	{
    	return -1; // same error as in read
  	}

	struct pipe_buf *pipe = (struct pipe_buf *)filep->data;
	int nwrite = 0;
	while(nwrite <= count)
	{
		nwrite ++;
		pipe->buf[pipe->end] = (unsigned char)*buf++;
		pipe->end = (pipe->end + 1) % PIPEBUF_SIZE;//move and wrap around if needed
	}

	return nwrite;
}



int pipe_readdir(struct file* filep, void* buf, unsigned int count)
{
  return -1; // this is an illegal operation
} 
