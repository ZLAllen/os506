#include <sys/tarfs.h>
#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/system.h>
#include <sys/kmalloc.h>
#include <sys/fs.h>
#include <sys/kstring.h>

//file operations table
struct file_ops tfs_file_ops =
{
    open: tfs_open,
    read: tfs_read,
    close: tfs_close
};

// initialize a root node here
struct posix_header_ustar root = {{0}};


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
        kprintf("size of header: %x, size of file %x\n", sizeof(struct posix_header_ustar), size);
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


//open a tarfs file
struct file *tfs_open(const char *fpath, int flags) 
{
    	//kprintf("tarfs open\n");
	struct file *filep;
	if (!fpath)
	{
		//kprintf("path name is NULL\n");
		return NULL;
	}
	// check for read only operations
	if(flags & (O_RDWR | O_WRONLY | O_CREAT | O_TRUNC))
	{
		//kprintf("ERROR: write operations not allowed\n");
		return NULL;
	}
	struct posix_header_ustar *hdr;
	//iterate tarfs section till file is found
        hdr = get_tfs_first();
        kprintf("first: %s\n", hdr->name);
	while(hdr != NULL)
	{      
              //  kprintf("path %s vs hdr name %s\n", path, hdr->name, sizeof(path), memcmp(path, hdr->name, sizeof(hdr->name)));
                kprintf("path %s vs hdr name %s\n", fpath, hdr->name);
		if(memcmp(fpath, hdr->name, kstrlen(fpath)) == 0) 
		{
			kprintf("found the matching file\n");
                      
			filep = kmalloc(); 
			filep->private_data = hdr;
			filep->f_op = &tfs_file_ops;
                  filep->f_pos = (uint64_t)get_tfs_next(hdr);
                  filep->f_flags = flags;//??dont know values
                  filep->f_count = 1;
                  filep->f_size = oct_to_bin(hdr->size, sizeof(hdr->size));
                        print_tfs(hdr);
			return filep;
		}	
                hdr = get_tfs_next(hdr);
	}
  while(1);
	return NULL;
}




//reads a tarfs file 
ssize_t tfs_read(struct file *filep, char *buf, size_t count, off_t *offset)
{
	//kprintf("\ntarfs read\n");
        
        if (filep->f_flags == 5)
        {
            //kprintf("filep represents a directory\n");
            return -1;
        }
	if(*offset == filep->f_size || count == 0)
	{
		//kprintf("reading 0 bytes\n");
		return 0;
	}
        struct posix_header_ustar *hdr = (struct posix_header_ustar *)filep->private_data;
        ssize_t size, to_read;
	size = filep->f_size - *offset;
        //count should be minimum of available size and count
	to_read = (size < count) ? size:count;
        kprintf("\nbytes_left=%d, offset=%d, name= %s, num_read=%d, count=%d\n", size, *offset, hdr->name, to_read, count);
	memcpy( (char *)(hdr+1), buf, to_read);//??
	*offset += to_read;
	return to_read;
}

//closes a tarfs file
int tfs_close(struct file *filep)
{
	//kprintf("tarfs close\n");
	if (!filep)
	{
		//kprintf("file is NULL\n");
		return -1;
	}
        filep->f_count--;
        if(filep->f_count == 0)
        {
	    memset(filep, 0, sizeof(struct file));		
	    kfree(filep);
        }
	return 0;
}


/*
helper functions
*/

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
	kprintf("Type: %s\n", hdr->typeflag);
    	kprintf("File Name: %s\n", hdr->name);	
    	kprintf("Filesystem type: %s\n", hdr->magic);
        kprintf("Size: %d", oct_to_bin(hdr->size, sizeof(hdr->size)));
	/*
        kprintf("Prefix: %s\n", hdr->prefix);
	kprintf("uid: %s\n", hdr->uid);
	kprintf("gid: %s\n", hdr->gid);
	kprintf("mode: %s\n", hdr->mode);
	kprintf("version: %s\n", hdr->version);
	kprintf("devmajor: %s\n", hdr->devmajor);
	kprintf("devminor: %s\n", hdr->devminor);
	*/
    }
    return 0;
}





/*
 * To extract directory from tarfs, not to be confused with tarfs read
 * return number of bytes read 
 * may return files or directory, as requested b * ls most of the time
 * */
// TODO define a root node with a name "/"

/*
int tarfs_readdir(struct file *filep, void *buf, int count)
{

    // the readdir approach is different, we are trying to record the 
    // neccerary information for linux dirent, therefore it should be
    // selective

    // first of all we fetch the header
    struct posix_header_ustar *hdr = (struct posix_header_ustar *)filep->private_data;

    // my inituition: files under one directory share the fhdr
    // check fhdr for possible end of directory
    //



}
*/
