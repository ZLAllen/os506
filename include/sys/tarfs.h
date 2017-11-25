#ifndef _TARFS_H
#define _TARFS_H

#include <sys/defs.h>
#include <sys/system.h>
#include<sys/kprintf.h>

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

struct posix_header_ustar {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12]; //ASCII octal string
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};



enum {O_RDONLY = 0, O_WRONLY = 1, O_RDWR = 2, O_CREAT = 0x02, O_TRUNC = 0x04, O_DIRECTORY = 0x10000 };


#define TFS_FILE1 0
#define TFS_FILE2 '\0'
#define TFS_DIR 5


//octal to binary
uint64_t oct_to_bin(char *optr, int length);
int print_tfs_metadata(struct posix_header_ustar *hdr);

//functions
struct file *tfs_open(const char *path, int flags);
int tfs_read(struct posix_header_ustar *hdr, char *buf, size_t count, off_t *offset);
int tfs_close(struct file *fp);


#endif
