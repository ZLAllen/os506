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
		kprintf("filep is NULL\n");
		return -1;
	}
	Elf64_Ehdr *ehdr;
	int valid;

	ehdr = (Elf64_Ehdr *)(((char *)filep->private_data) + sizeof(struct posix_header_ustar));
	valid = identify_ehdr(ehdr);
	if(valid != 0)
	{
		kprintf("not valid elf header\n");
		return -1;
	}
	valid = identify_phdr(ehdr);
	if(valid != 0)
	{
		kprintf("not valid program header\n");
		return -1;
	}
	kprintf("valid elf object\n");
        //print_elf(ehdr);
	return 0;
	
}

int identify_phdr(Elf64_Ehdr *ehdr)
{
	Elf64_Phdr *phdr;
	for(int n = 0; n < ehdr->e_phnum; n++) 
	{
		phdr = get_phdr(n, ehdr);//TODO exec files have program header
		if(!phdr)
		{
			kprintf("object has no program header\n");
			return -1;
		}
	}
	return 0;
}


int identify_ehdr(Elf64_Ehdr *ehdr)
{
	if(!ehdr) //NULL check
	{
		kprintf("header is NULL\n");
		return -1;

	}
	if(ehdr->e_type != 2) //check if executable file
	{
		kprintf("\nnot an exec file\n");
		return -1;
	}
	if (memcmp(ehdr->e_ident, "\177ELF", 4) != 0) //check magic numbers "0x7F E L F" to octal
	{
		kprintf("magic numbers dont match\n");
		return -1;
	}
	return 0;
}



int load_elf(struct file *filep, struct mm_struct *mm)
{	
	if (!filep)
	{
		kprintf("filep is NULL\n");
		return -1;
	}
	if (!mm)
	{
		kprintf("mm is NULL\n");
	}
	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	ehdr = get_ehdr(filep);
	//ehdr->e_entry address to which the system first transfers control, starting the process
	for(int n = 0; n < ehdr->e_phnum; n++)
	{
		phdr = get_phdr(n, ehdr);
		if(phdr->p_type == 1)
		{
			kprintf("will handle phdr now\n");
		}
	}
	
	return 0;
}


Elf64_Phdr* get_phdr(int ph_num, Elf64_Ehdr *ehdr)
{
	Elf64_Phdr* phdr;
	phdr = (Elf64_Phdr *)(((uint64_t)ehdr) + ehdr->e_phoff + ph_num);//program header offset + program num
	if(!phdr)
	{
		kprintf("phdr is NULL\n");
		return NULL;
	}
	return phdr;
}


Elf64_Ehdr* get_ehdr(struct file *filep)
{
	Elf64_Ehdr *ehdr;
	ehdr = (Elf64_Ehdr *)(((char *)filep->private_data) + sizeof(struct posix_header_ustar));
	if (!ehdr)
	{
		kprintf("header is NULL\n");
		return NULL;
	}
	return ehdr;
}


Elf64_Shdr* get_shdr(int sh_num, Elf64_Ehdr *ehdr) 
{
    if(!ehdr)
    {
	kprintf("ehdr is NULL\n");
	return NULL;
    }
    Elf64_Shdr *shdr;
    shdr = (Elf64_Shdr *)(((uint64_t)ehdr) + ehdr->e_shoff + sh_num);//section header offset + section num
    return shdr;
}


void print_elf(Elf64_Ehdr* ehdr)
{
	kprintf("*** Displaying ELF info ***\n");
	
	kprintf("ELF Header\n");
	kprintf("e_ident[0]: %s\n", ehdr->e_ident);
	kprintf("e_type: %d\n", ehdr->e_type);
	kprintf("e_entry: %p\n", ehdr->e_entry);

	kprintf("Program Header\n");
	Elf64_Phdr *phdr;
	for(int n = 0; n < ehdr->e_phnum; n++)
	{
		phdr = get_phdr(n, ehdr);
		print_phdr(phdr);
	} 

	kprintf("Section Header\n");
	Elf64_Shdr *shdr;
	for(int n = 0; n < ehdr->e_shnum; n++) 
	{
		shdr = get_shdr(n, ehdr);
		print_shdr(ehdr, shdr);
	}
}

char *elf_lookup_string(Elf64_Ehdr *ehdr, Elf64_Word sh_name) 
{ 
    char *name = (char*)ehdr + get_shdr(ehdr->e_shstrndx, ehdr)->sh_offset;
    if(name == NULL)
    {
        kprintf("name is NULL\n");
	return NULL;
    }
    return name + sh_name;
}

//print program header fields
void print_phdr(Elf64_Phdr *phdr)
{
	if(!phdr)
	{
		kprintf("PHDR is NULL.\n");
		return ;
	}

	kprintf("p_type: %d\n", phdr->p_type);
	kprintf("p_offset: %d\n", phdr->p_offset);
	
}

//print section header fields
void print_shdr(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr)
{
	char *name = elf_lookup_string(ehdr, shdr->sh_name);
	kprintf("section name %s", name);
}
