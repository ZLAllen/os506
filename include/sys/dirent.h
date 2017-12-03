#ifndef _DIRENT_H
#define _DIRENT_H


/*
https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/unix/dirstream.h;
h=8303f07fab6f6efaa39e51411ef924e712d995e0;hb=fa39685d5c7df2502213418bead44e9543a9b9ec
*/
struct dstream
{
        int fd;
        size_t size;
        size_t offset;
	char data[100]; //size?
	
};

struct dirent 
{
    ino_t d_ino;       /* inode number */
    off_t d_off;       /* offset to the next dirent */
    unsigned short d_reclen;    /* length of this record */
    unsigned char d_type;      /* type of file; not supported by all file system types */
    char d_name[256]; /* filename */
};


void *opendir(const char *dirname);
struct dirent *readdir(struct dstream *dirp);
int closedir(struct dstream *dirp);



struct linux_dirent {
    unsigned long d_ino;
    unsigned long d_off;
    unsigned short d_reclen; 
    unsigned char d_type;
    char          d_name[];
};








#endif
