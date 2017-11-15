#include <sys/tarfs.h>
#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/system.h>

#define MIN(x, y) (((x)<(y))?(x):(y))

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
        uint64_t size = oct_to_bin(hdr->size, sizeof(hdr->size));
        hdr += 1 + size/512 + (size % 512 != 0); //512 byte sectors
        if (hdr->name[0]== '\0')
        {
                kprintf("header name is NULL\n");
                return NULL;
        }
        return hdr;
}


//open a tarfs file
struct posix_header_ustar *tfs_open(const char *path, int flags) 
{
	kprintf("tarfs open\n");
	if (! path)
	{
		kprintf("path name is NULL\n");
		return NULL;
	}
	// check for read only operations
	if(flags & (O_RDWR | O_WRONLY | O_CREAT | O_TRUNC))
	{
		kprintf("ERROR: write operations not allowed");
		return NULL;
	}
	struct posix_header_ustar *hdr;
	//iterate tarfs section till file is found
	for(hdr = get_tfs_first(); hdr != NULL; hdr = get_tfs_next(hdr))
	{
		if(memcmp(path+1, hdr->name, sizeof(hdr->name)) == 0)
		{
			kprintf("found the file");
			return hdr;
		}
		kprintf("found file: %s\n", hdr->name);
		//print_tfs_metadata(hdr);
	}
	return NULL;
}


//reads a tarfs file 
int tfs_read(struct posix_header_ustar *hdr, char *buf, size_t count, off_t *offset)
{
	kprintf("tarfs read");
	size_t bytes_left, bytes_to_read;
	char *data_begin;
	unsigned long f_size = oct_to_bin(hdr->size, sizeof(hdr->size));
	if(*offset == f_size || count == 0)
	{
		kprintf("reading 0 bytes");
		return 0;
	}
	bytes_left = f_size - *offset;
	bytes_to_read = MIN(bytes_left, count);
	data_begin = (char *)(hdr + 1);
	memcpy(buf, *offset + data_begin, bytes_to_read);
	*offset += bytes_to_read;
	return bytes_to_read;
}

// octal string to integer
uint64_t oct_to_bin(char *ostr, int length)
{
    int num = 0;
    char *c = ostr;
    while (length-- > 0) 
    {
        num *= 8;
        num += *c - '0';
        c++;
    }
    return num;
}

/*
//print hdr metadata
int print_tfs_metadata(struct posix_header_ustar *hdr)
{
    if(hdr)
    {
	if(hdr->typeflag == '0')
		kprintf("Normal File");
	if (hdr->typeflag == '5')
	     kprintf("Directory");
 
    	kprintf("File Name: %s\n", hdr -> name);
    	kprintf("Magic number: %s\n", hdr -> magic);
    }
    return 0;
}
*/
