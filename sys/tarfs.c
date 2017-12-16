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
void set_dirent(struct linux_dirent *dent, struct posix_header_ustar *hdr, char *dname, unsigned short size);


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
struct posix_header_ustar root_hdr = {{0}};//TODO shouldn't we set type to '5' and name to "/"


//open a tarfs file/directory
struct file *tfs_open(const char *fpath, int flags) 
{
    //kprintf("tarfs open\n");
	struct file *filep;
	if (!fpath)
		return NULL;
	int root = 0;
	// check for read only operations
	if (flags & (O_WRONLY | O_RDWR))//handling 3 flags only
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
			
			if(hdr->typeflag[0] != TFS_DIR && flags & O_DIRECTORY)//opendir asks for a dir but hdr is a file case
			{
				kprintf("hdr is a file but flags say directory\n");
				return NULL;
			}
			
			kprintf("found the matching file\n");
			filep = kmalloc();
			filep->data = hdr;
			filep->flags = flags;
			filep->fop = &tfs_file_ops;
			if (root == 0)
			{
				if (hdr->typeflag[0] == TFS_DIR) //directory points to the next
            		*(filep->offset) = (uint64_t)get_tfs_next(hdr);
				else
					*(filep->offset) = 0;//file points to nothing
			}
			else
				*(filep->offset) = (uint64_t)hdr;//root points to the first hdr
           
            filep->count = 1;
            filep->size = oct_to_bin(hdr->size, sizeof(hdr->size));
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
	if (hdr->typeflag[0] == TFS_DIR)//can't read a directory
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



//print hdr metadata
int print_tfs(struct posix_header_ustar *hdr)
{
    if(hdr)
    {
		kprintf("type: %s; name: %s; size: %d\n", hdr->typeflag, hdr->name, oct_to_bin(hdr->size, sizeof(hdr->size)));
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
	struct posix_header_ustar *next_hdr = (struct posix_header_ustar *)(*(filep->offset));//get the offset- first dstream
	
	while(next_hdr != NULL) //desired dirent struct
	{
		kprintf("next hdr name: %s", next_hdr->name);
		
		if(!is_root && memcmp(hdr->name, next_hdr->name, kstrlen(hdr->name)) != 0)//check if no more
			break;


		// 3. how to find the right hdr for the dirent fields
		char *dname = kstrlen(name) +  next_hdr->name;
		kprintf("drent name: %s", dname);

		if(memchr(dname, '/', kstrlen(dname)))//check if need to skip to the next one
			continue;

	    //TODO check if this is right		   
		unsigned int size = (unsigned int) ((size_t)&(((struct linux_dirent *)0)->d_name) + kstrlen(dname));
		if(size < count)
		{
			*(filep->offset) = (uint64_t) next_hdr;//no more
			return -1;
		}
		
		
		struct linux_dirent *drent = buff;//buff holds the dirent struct
		set_dirent(drent, next_hdr, dname, (unsigned short)size);

		*(filep->offset) = (uint64_t) get_tfs_next(next_hdr);//go to next one

		next_hdr = get_tfs_next(next_hdr);
		return size;
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


void set_dirent(struct linux_dirent *drent, struct posix_header_ustar *next_hdr, char *dname, unsigned short size)
{
	
	//set the fields
	memcpy(dname, drent->d_name, kstrlen(dname));	

	drent->d_off = 0;
	drent->d_reclen = size;
	char typeflag = next_hdr->typeflag[0];
	if (typeflag == '5')
		drent->d_type = DT_DIR;	
	else if (typeflag == '0' || typeflag == '\0')
		drent->d_type = DT_REG;
	else
		drent->d_type = DT_UNKNOWN;//all others except directory and file as unknown

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


