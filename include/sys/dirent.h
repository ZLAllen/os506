#ifndef _DIRENT_H
#define _DIRENT_H


#define MAX_NAME 256
#define BUFF_SIZE 204

//dirent types
#define DT_DIR 4
#define DT_REG 8
#define DT_UNKNOWN 0

struct dstream
{
	int fd;
    size_t size;
    size_t offset;
	char buff[BUFF_SIZE]; 
	
};



struct linux_dirent {
    unsigned long d_ino;
    unsigned long d_off;
    unsigned short d_reclen; 
    unsigned char d_type;
    struct linux_dirent* free;
    char d_name[MAX_NAME];
};



struct dstream *opendir(const char *name);
struct dirent *readdir(struct dstream *dirp);
int closedir(struct dstream *dirp);

int getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count);







#endif
