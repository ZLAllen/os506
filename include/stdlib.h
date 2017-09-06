#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/defs.h>

extern char** environ;

int main(int argc, char *argv[], char *envp[]);
void exit(int status);
int wait(int *status);

void *malloc(size_t size);
void free(void *ptr);

#endif
