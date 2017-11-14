#include <sys/tarfs.h>
#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/system.h>
//#include <string.h>


//open a tarfs file
struct file *tfs_open(const char *path, int flags) 
{
	kprintf("tarfs open\n");
	if (! path)
	{
		kprintf("path name is NULL\n");
		return NULL;
	}
	struct posix_header_ustar *hd;
	//iterate tarfs section till file is found
	for(hd = get_tfs_first(); hd != NULL; hd = get_tfs_next(hd))
	{
		/*if(strncmp(path+1, hd->name, sizeof(hd->name)) == 0)
		{
			kprintf("found the file");
		}*/
		kprintf("found the file: %s\n", hd->name);
	}
	return NULL;


}

// octal string to integer
uint64_t otoi(char *ostr, int length) 
{
	uint64_t inum = 0;
	if (ostr != NULL)
	{
		uint64_t inum = 0;
		while(length-- > 0 && *ostr <= '7' && *ostr >= '0') 
		{
			inum <<= 3;
			inum += *ostr++ - '0';
		}
	}
	return inum;
}

