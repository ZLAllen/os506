#ifndef _TARFS_H
#define _TARFS_H

#include <sys/defs.h>
#include <sys/system.h>
#include<sys/kprintf.h>
#include <sys/fs.h>


extern char _binary_tarfs_start;
extern char _binary_tarfs_end;
extern struct file_ops tfs_file_ops;


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
#define TFS_HLINK 1
#define TFS_SYMLINK 2
#define TFS_CHAR_DEVICE 3
#define TFS_BLK_DEVICE 4
#define TFS_DIR 5



//octal to binary
uint64_t oct_to_bin(char *optr, int length);
int print_tfs(struct posix_header_ustar *hdr);

//functions

struct posix_header_ustar *get_tfs_first(void);
struct file *tfs_open(const char *fpath, int flags); //int open(struct inode *inode,struct file *file)
ssize_t tfs_read(struct file *filep, char *buf, size_t count, off_t *offset);
int tfs_close(struct file *filep);

int tfs_opendir(const char *fpath, int flags);
int tfs_readdir(struct file *filep, void *buf, size_t count);
int tfs_closedir(struct file *filep);

#endif
