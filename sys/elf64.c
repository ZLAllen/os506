#include <sys/elf64.h>
#include <sys/system.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/task_pool.h>
#include <sys/fs.h>



Elf64_Ehdr* get_ehdr(struct file *filep); 
Elf64_Phdr* get_phdr(Elf64_Ehdr *ehdr);
Elf64_Shdr* get_shdr(Elf64_Ehdr *ehdr);

int check_ehdr(Elf64_Ehdr *ehdr);
int get_shdr_info(Elf64_Ehdr *ehdr);
int get_phdr_info(Elf64_Ehdr *ehdr);

void print_elf(Elf64_Ehdr* ehdr);
void print_shdr(Elf64_Shdr *shdr);
void print_phdr(Elf64_Phdr *phdr);


// METHOD 1: parses the file and performs validation
int parse_elf(struct file *filep)
{
	if(!filep)
	{
		kprintf("filep is NULL\n");
		return -1;
	}
        // get the elf header
	Elf64_Ehdr *ehdr = get_ehdr(filep);

	int valid = 0;
        
        valid = check_ehdr(ehdr);
        valid = get_phdr_info(ehdr);
        valid = get_shdr_info(ehdr);

        if(valid == 0)
        {
            kprintf("valid ELF object\n");
            //print_elf(ehdr);
        }
        return valid;	
}



//METHOD 2: load txt, data, and bss section into memory and sets the entry point in task 
int load_elf(struct file *filep, struct mm_struct *mm)
{
       	/*if (!mm)
	{
		kprintf("mm is NULL\n");
	}*/
	Elf64_Ehdr *ehdr = get_ehdr(filep);
        
        uint64_t start_vaddr, end_vaddr, vm_type; 
        Elf64_Phdr* phdr = (Elf64_Phdr*) ((void*)ehdr + ehdr->e_phoff);
        int size;
        //for each program header
        for (int n = 0; n < ehdr->e_phnum; ++n)
        {
            if ((int)phdr->p_type == 1)
            {
                start_vaddr    = phdr->p_vaddr;
                size           = phdr->p_memsz;
                end_vaddr      = start_vaddr + size; 
                kprintf("start vaddress: %d and end vaddress: %d", start_vaddr, end_vaddr);
                if (phdr->p_flags == 5)
                {
                    vm_type = 5;
                }
                else if (phdr->p_flags == 6)
                {
                    vm_type = 6;
                }
                else
                {
                    vm_type = -1;
                }
                kprintf("vm_type set to: %d\n", vm_type);
                // allocate a new vma and add to mm_struct
                
                //load elf into vmm: mmap and memcpy and memset for .bss section
                
                
            }
        }
        

        //schedule process
	
	return 0;
}



/* HELPER FUNCTIONS
*/


int get_phdr_info(Elf64_Ehdr *ehdr)
{
	int valid = 0;
        Elf64_Phdr* phdr = get_phdr(ehdr);
        if(!phdr)
        {
            kprintf("phdr is NULL\n");
            valid = -1;
        }
        for (int i = 0; i < ehdr->e_phnum; ++i) 
        {
            kprintf("\nSegment type: %x\n", phdr->p_type);
            if ((int)phdr->p_type == 1)
            {
                kprintf("1 found!!\n");
            }
        }
        return valid;
}

int get_shdr_info(Elf64_Ehdr *ehdr)
{
        int valid = 0;
        Elf64_Shdr* shdr =  get_shdr(ehdr);
        if(!shdr)
        {
            //kprintf("shdr is NULL\n");
            valid = -1;
        }
        Elf64_Shdr* stable_hdr = (Elf64_Shdr*)((void *) shdr + (ehdr->e_shentsize * ehdr->e_shstrndx)); 
        char* stable = (char*)((void *) ehdr + stable_hdr->sh_offset);
        for(int n = 0; n < ehdr->e_shnum; ++n)
        {
            //kprintf("\n...%x....%x", shdr->sh_type, shd->sh_offset); 
            char *p = (char*)((void *) stable + shdr->sh_name);  
            kprintf("\nsection name: %s\n", p); 
            shdr = shdr + 1;
        }
        return valid;
}


int check_ehdr(Elf64_Ehdr *ehdr)
{
	if(!ehdr) 
	{
		//kprintf("header is NULL\n");
		return -1;

	}
        if (memcmp(ehdr->e_ident, "\177ELF", 4) != 0) //check magic numbers "0x7F E L F" to octal
	{
		//kprintf("magic numbers dont match\n");
		return -1;
	}
	if(ehdr->e_type != 2) //check if executable file
	{
		//kprintf("\nnot an exec file\n");
		return -1;
	}	
	return 0;
}


Elf64_Phdr* get_phdr(Elf64_Ehdr *ehdr)
{
	Elf64_Phdr * phdr = (Elf64_Phdr*)((void*)ehdr + ehdr->e_phoff);	
	return phdr;
}


Elf64_Ehdr* get_ehdr(struct file *filep)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)(((char *)filep->private_data) + sizeof(struct posix_header_ustar));	
	return ehdr;
}


Elf64_Shdr* get_shdr(Elf64_Ehdr *ehdr) 
{
    Elf64_Shdr *shdr = (Elf64_Shdr*)((void *) ehdr + ehdr->e_shoff); 
    return shdr;
}


void print_elf(Elf64_Ehdr* ehdr)
{

	kprintf("ELF Header\n");
	kprintf("e_ident[0]: %s\n", ehdr->e_ident);
	kprintf("e_type: %d\n", ehdr->e_type);
	kprintf("e_entry: %p\n", ehdr->e_entry);

	kprintf("Program Header\n");
	for(int n = 0; n < ehdr->e_phnum; n++)
	{
		Elf64_Phdr *phdr = get_phdr(ehdr);
                if(phdr->p_type == 1) //loadable section
		    print_phdr(phdr);
	} 

	kprintf("Section Header\n");
	for(int n = 0; n < ehdr->e_shnum; n++) 
	{
		Elf64_Shdr *shdr = get_shdr(ehdr);
		print_shdr(shdr);
	}
}


//print program header fields
void print_phdr(Elf64_Phdr *phdr)
{
	kprintf("p_type: %d\n", phdr->p_type);
	kprintf("p_offset: %d\n", phdr->p_offset);
	
}

//print section header fields
void print_shdr(Elf64_Shdr *shdr)
{
	kprintf("section name\n");
}
