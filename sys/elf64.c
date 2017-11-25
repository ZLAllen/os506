#include <sys/elf64.h>
#include <sys/system.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/task_pool.h>
#include <sys/fs.h>

int identify_elf(struct file *filep)
{
	if(!filep)
	{
		kprintf("filep is NULL");
		return -1;
	}
	Elf64_Ehdr *hdr;
	int valid;

	hdr = get_hdr(filep);
	valid = identify_hdr(hdr);
	if(valid != 0)
	{
		kprintf("not valid elf header");
		return -1;
	}
	valid = identify_phdr(hdr);
	if(valid != 0)
	{
		kprintf("no program header");
		return -1;
	}
	kprintf("validated elf object");
	return 0;
	
}

int identify_phdr(Elf64_Ehdr *hdr)
{
	Elf64_Phdr *phdr;
	for(int n = 0; n < hdr->e_phnum; n++) 
	{
		phdr = get_phdr(n, hdr);//TODO exec files have program header
		if(phdr)
		{
			kprintf("object has program header");
			return 0;
		}
	}
	return -1;
}


int identify_hdr(Elf64_Ehdr *hdr)
{
	if(!hdr) //NULL check
	{
		kprintf("header is NULL");
		return -1;

	}
	if(hdr->e_type != 2) //check if executable file
	{
		kprintf("not an exec file");
		return -1;
	}
	if (memcmp(hdr->e_ident, "177ELF", 4) != 0) //check magic numbers "0x7F E L F" to octal
	{
		kprintf("magic numbers dont match");
		return -1;
	}
	return 0;
}



int load_elf(struct file *filep, struct mm_struct *mm)
{	
	if (!filep)
	{
		kprintf("filep is NULL");
		return -1;
	}
	if (!mm)
	{
		kprintf("mm is NULL");
	}
	Elf64_Ehdr *hdr;
	Elf64_Phdr *phdr;
	hdr = get_hdr(filep);
	//hdr->e_entry address to which the system first transfers control, starting the process
	for(int n = 0; n < hdr->e_phnum; n++)
	{
		phdr = get_phdr(n, hdr);
		if(phdr->p_type == 1)
		{
			kprintf("will handle phdr now");
		}
	}
	
	return 0;
}


Elf64_Phdr* get_phdr(int ph_num, Elf64_Ehdr *hdr)
{
	Elf64_Phdr* phdr;
	phdr = (Elf64_Phdr *)(((uint64_t)hdr) + hdr->e_phoff + ph_num);
	if(!phdr)
	{
		kprintf("phdr is NULL");
		//return NULL;
	}
	return phdr;
}

Elf64_Ehdr* get_hdr(struct file *filep)
{
	Elf64_Ehdr *hdr;
	hdr = (Elf64_Ehdr *)(((char *)filep->private_data) + sizeof(struct posix_header_ustar));
	if (!hdr)
	{
		kprintf("header is NULL");
		//return NULL;
	}
	return hdr;
}

void print_elf(Elf64_Ehdr* hdr)
{
	kprintf("*** Displaying ELF info ***\n");
	
	kprintf("ELF Header\n");
	kprintf("e_ident[0]: %s\n", hdr->e_ident);
	kprintf("e_type: %d\n", hdr->e_type);
	kprintf("e_entry: %p\n", hdr->e_entry);

	Elf64_Phdr *phdr;
	for(int n = 0; n < hdr->e_phnum; n++)
	{
		phdr = get_phdr(n, hdr);
		print_phdr(phdr);
	} 
}



void print_phdr(Elf64_Phdr *phdr)
{
	if(!phdr)
	{
		kprintf("PHDR is NULL.\n");
		return;
	}
	kprintf("Program Header\n");

	kprintf("p_type: %d\n", phdr->p_type);
	kprintf("p_offset: %d\n", phdr->p_offset);

	
	
}

