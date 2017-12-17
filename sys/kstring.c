#include <sys/kstring.h>


//actual length: excluding NULL
int kstrlen(const char *str)
{
    int length=0;
    while (*str++ != '\0')
        length++;
    return length;
}

//NULL terminated
char * kstrcpy(const char *src, char *dest)
{
    char *str = dest;
    while (*src) 
	{
        *dest++ = *src++;
    }
    *dest = '\0';
    return str;
}


