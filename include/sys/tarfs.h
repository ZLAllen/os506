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
  char size[12];
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

//octal to integer
uint64_t otoi(char *optr, int length);
struct file *tfs_open(const char *path, int flags);

// ptr to the first tarfs header 
static inline struct posix_header_ustar *get_tfs_first(void) 
{
	if(&_binary_tarfs_end - &_binary_tarfs_start < 512)
	{
		kprintf("tarfs end-start is < 512\n");
		return NULL;
	}
	return (struct posix_header_ustar *)&_binary_tarfs_start;	
}


//ptr to the next tarf header
static inline struct posix_header_ustar *get_tfs_next(struct posix_header_ustar *hdr) 
{
	if (!hdr)
	{
		kprintf("header is NULL\n");
		return NULL;
	}
	else if (hdr->name[0] == '\0')
	{
		kprintf("header name is NULL\n");
		return NULL;
	}
	uint64_t size = otoi(hdr->size, sizeof(hdr->size));
	hdr += 1 + size/512 + (size % 512 != 0);
	if (hdr->name[0]== '\0')
	{
		kprintf("header name is NULL\n");
		return NULL;
	}
	return hdr;
}


#endif
