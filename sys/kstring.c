#include <sys/kstring.h>


int kstrlen(const char *str)
{
    int length=0;
    while (*str++ != '\0')
        length++;
    return length;
}


