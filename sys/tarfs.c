#include <sys/tarfs.h>
#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/system.h>


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
        hdr += 1 + size/512 + (size % 512 != 0);
        if (hdr->name[0]== '\0')
        {
                kprintf("header name is NULL\n");
                return NULL;
        }
        return hdr;
}


//open a tarfs file
struct file *tfs_open(const char *path, int flags) 
{
	kprintf("tarfs open\n");
	if (! path)
	{
		kprintf("path name is NULL\n");
		return NULL;
	}
	struct posix_header_ustar *hdr;
	//iterate tarfs section till file is found
	for(hdr = get_tfs_first(); hdr != NULL; hdr = get_tfs_next(hdr))
	{
		if(memcmp(path+1, hdr->name, sizeof(hdr->name)) == 0)
		{
			kprintf("found the file");
		}
		kprintf("found file: %s\n", hdr->name);
		//print_tfs_metadata(hdr);
	}
	return NULL;


}


// closes a tarfs file
int tfs_close()
{
	kprintf("tarfs close");
	return 0;
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
