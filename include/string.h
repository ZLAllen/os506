#ifndef _STRING_H
#define _STRING_H

#include <sys/defs.h>

int strcmp(const char *str1, const char *str2);
char *strtok(char *str, const char *delimiters);
size_t strlen(const char *str);
char* strcpy(char *target, const char *src);
char* strncpy(char *target, const char *src, int n);
char *strcat(char *dest, const char *src);

#endif
