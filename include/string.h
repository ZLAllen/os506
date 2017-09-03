#ifndef _STRING_H
#define _STRING_H

#include <sys/defs.h>

int strcmp(const char *str1, const char *str2);
char *strtok(char *str, const char *delimiters);
size_t strlen(const char *str);

#endif
