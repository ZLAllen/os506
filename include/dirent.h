#ifndef _DIRENT_H
#define _DIRENT_H


#include <sys/defs.h>

#define MAX_NAME 256
#define BUFF_SIZE 2048

//dirent types
#define DT_DIR 4
#define DT_REG 8
#define DT_UNKNOWN 0

struct dstream
{
	int fd;
    size_t size;//nbytes to be read
    size_t offset;//ofset into the buff
	char buff[BUFF_SIZE];//this holds the dirent struct 
	
};



struct linux_dirent 
{
    unsigned long d_ino;
    unsigned long d_off;
    unsigned short d_reclen; 
    unsigned char d_type;
    struct linux_dirent* free;
    char d_name[MAX_NAME];
};



struct dstream *opendir(const char *name);
struct linux_dirent *readdir(struct dstream *dirp);
int closedir(struct dstream *dirp);




#endif
