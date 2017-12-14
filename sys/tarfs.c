#include <sys/tarfs.h>
#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/system.h>
#include <sys/kmalloc.h>
#include <sys/files.h>
#include <sys/kstring.h>
#include <dirent.h>
#include <fcntl.h>

struct posix_header_ustar *get_tfs_first(void);
struct posix_header_ustar *get_tfs_next(struct posix_header_ustar *hdr);
uint64_t oct_to_bin(char *ostr, int length);
int print_tfs(struct posix_header_ustar *hdr);
struct linux_dirent *create_dirent(struct posix_header_ustar *hdr, unsigned short size);


//we provide these operations on the file object
struct file_ops tfs_file_ops =
{
    open: tfs_open,
    read: tfs_read,
    close: tfs_close,
	readdir: tfs_readdir,
	closedir: tfs_closedir
};

// initialize a root node here
struct posix_header_ustar root_hdr = {{0}};


//open a tarfs file
struct file *tfs_open(const char *fpath, int flags) 
{
    //kprintf("tarfs open\n");
	struct file *filep;
	if (!fpath)
		return NULL;
	int root = 0;
	// check for read only operations
	if (flags & (O_WRONLY | O_RDWR))
	{
		kprintf("write operations not allowed\n");
		return NULL;
	}
	if(fpath[0] == '/' && fpath[1] == '\0') //check for root
	{
		//kprintf("root\n");
		root = 1;
	}

	struct posix_header_ustar *hdr = get_tfs_first();
    //kprintf("first: %s\n", hdr->name);
	while(hdr != NULL)
	{      
        //  kprintf("path %s vs hdr name %s\n", path, hdr->name, sizeof(path), memcmp(path, hdr->name, sizeof(hdr->name))); 
		if(memcmp(fpath, hdr->name, kstrlen(fpath)) == 0) 
		{
			kprintf("found the matching file\n");
			filep = kmalloc();
			filep->data = hdr;
			filep->flags = flags;
			filep->fop = &tfs_file_ops;
			if (root == 0)
			{
				if (hdr->typeflag[0] == '5') //directory points to the next
            		filep->offset = (uint64_t)get_tfs_next(hdr);
				else
					filep->offset = 0;//file points to nothing
			}
			else
				filep->offset = (uint64_t)hdr;//root points to the first hdr
           
            filep->count = 1;
            filep->size = oct_to_bin(hdr->size, sizeof(hdr->size));
            //print_tfs(hdr);
			return filep;
		}	
        hdr = get_tfs_next(hdr);
	}
   
	return NULL;
}




//reads a tarfs file 
ssize_t tfs_read(struct file *filep, char *buff, size_t count, off_t *offset)
{
	struct posix_header_ustar *hdr = (struct posix_header_ustar *)filep->data;
	if (hdr->typeflag[0] == '5')//can't read a directory
		kprintf("trying to read a directory\n");

	size_t size  = filep->size - *offset;
	size_t acount = count<size?count:size;
	memcpy((char *) (hdr + 1) + *offset, buff, acount);
	*offset += acount;
	return 0;
}

//closes a tarfs file
int tfs_close(struct file *filep)
{
	if (!filep)	
		return -1;
    
	filep->count--;
    if(filep->count == 0)
    {
	    memset(filep, 0, sizeof(struct file));		
	    kfree(filep);
    }
	return 0;
}




// ptr to the first tarfs header 
struct posix_header_ustar *get_tfs_first(void)
{
        return (struct posix_header_ustar *)&_binary_tarfs_start;
}

//ptr to the next tarf header
struct posix_header_ustar *get_tfs_next(struct posix_header_ustar *hdr)
{
        if (!hdr)
        {
                //kprintf("header is NULL\n");
                return NULL;
        } 
        else if (hdr->name[0] == '\0')
        {
                //kprintf("header name is NULL\n");
                return NULL;
        }
        uint64_t size = oct_to_bin(hdr->size, sizeof(hdr->size));
        //kprintf("size of header: %x, size of file %x\n", sizeof(struct posix_header_ustar), size);
        if(size > 0)
          hdr += (511+size)/(512) + 1; //512 byte sectors
        else
          hdr += 1;
        if (hdr->name[0] == '\0')
        {
                //kprintf("header name is NULL\n");
                return NULL;
        }
        return hdr;
}


// octal string to integer
uint64_t oct_to_bin(char *ostr, int length)
{
    int num = 0;
    char *c = ostr;
    while (--length > 0) 
    {
        num <<= 3;
        num += *c - '0';
        c++;
    }
    return num;
}


//print hdr metadata
int print_tfs(struct posix_header_ustar *hdr)
{
    if(hdr)
    {
		kprintf("type: %s; name: %s; type: %s; size: %d\n", hdr->typeflag, hdr->name, hdr->magic, oct_to_bin(hdr->size, sizeof(hdr->size)));
    }
    return 0;
}





/*
 * To extract directory from tarfs, not to be confused with tarfs read
 * return number of bytes read 
 * may return files or directory, as requested b * ls most of the time
 * */
// define a root node with a name "/"


int tfs_readdir(struct file *filep, void *buff, unsigned int count)
{

    // the readdir approach is different, we are trying to record the 
    // neccerary information for linux dirent, therefore it should be
    // selective

    // first of all we fetch the header
    struct posix_header_ustar *hdr = (struct posix_header_ustar *)filep->data;
	char * name = hdr->name;
	kprintf("first hdr name: %s", name);

    // my inituition: files under one directory share the fhdr
    // check fhdr for possible end of directory
	int is_root = 0;
    if (hdr == &root_hdr)
	{
		//kprintf("ROOT dir");
		is_root = 1;
	}
	struct posix_header_ustar *next_hdr = (struct posix_header_ustar *)filep->offset;//get the offset- first dstream
	
	while(next_hdr != NULL) //desired dirent struct
	{
		kprintf("next hdr name: %s", next_hdr->name);
		
		if(!is_root && memcmp(hdr->name, next_hdr->name, kstrlen(hdr->name)) != 0)
			break;
		// 1. how to find the right hdr to get these dirent fields
		char *entryname = is_root == 1? next_hdr->name + kstrlen(name) :  next_hdr->name + kstrlen(name) + 1;
		kprintf("entry name: %s", entryname);
	    // 2. how is size getting calculated and why
		unsigned short size = 5;//(unsigned short) (__builtin_offsetof(struct linux_dirent, d_name) + kstrlen(entryname) + 1);
		
		struct linux_dirent *drent = create_dirent(next_hdr, size);
		
		buff = drent;//dirent you wanted				
		
		next_hdr = get_tfs_next(next_hdr);
	}	

	return 0;


}

int tfs_closedir(struct file *filep)
{
	struct posix_header_ustar *hdr = (struct posix_header_ustar *)filep->data;

	if (hdr->typeflag[0] != '5')
	{
		kprintf("not a dir, wont call closedir");
		return -1;
	}
	return tfs_close(filep);

}


struct linux_dirent *create_dirent(struct posix_header_ustar *next_hdr, unsigned short size)
{
	struct linux_dirent *drent = NULL;
	//set the fields
	drent->d_off = 0;
	drent->d_reclen = size;
	char typeflag = next_hdr->typeflag[0];
	if (typeflag == '5')
		drent->d_type = DT_DIR;	
	else if (typeflag == '0' || typeflag == '\0')
		drent->d_type = DT_REG;
	else
		drent->d_type = DT_UNKNOWN;//all others except directory and file as unknown

	return drent;
}


void test_tarfs() 
{
	kprintf("root hdr: %s\n", root_hdr);
		
	kprintf("reading tarfs\n");
	struct posix_header_ustar *hdr = get_tfs_first();
    while(hdr != NULL)  
	{
        print_tfs(hdr);
		hdr = get_tfs_next(hdr);
    }
    kprintf("done reading tarfs\n");
}

