#ifndef __KPRINTF_H
#define __KPRINTF_H


#include <sys/defs.h>

void kprintf(const char *fmt, ...);
void kputchar(const char c);
void kputs(const char* str);
void clr();

void update_time(uint32_t day, uint32_t sec);

//terminal 
struct file* term_open(const char*, int); // return terminal file object
int term_close(struct file*); // return close status
ssize_t term_read(struct file*, char*, size_t, off_t*); // return num of bytes read
ssize_t term_write(struct file*, char*, size_t, off_t*); // return num of bytes written
int term_readdir(struct file*, void*, unsigned int); // return error 

int term_putchar(char a); // called by keyboard interrupt to place a character inside term_buf

// defines

#define BASE (KERN + 0xb8000)
#define EOT 4  //end of transmission
#endif
