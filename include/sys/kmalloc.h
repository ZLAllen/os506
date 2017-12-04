#ifndef _KMALLOC_H_
#define _KMALLOC_H_

void* get_kern_free_addr();

void set_kern_free_addr(void* ptr);


void* get_kern_temp_addr();


void set_kern_temp_addr(void* ptr);

void free_temp();


void* kmalloc();

void kfree(void* ptr);
#endif
