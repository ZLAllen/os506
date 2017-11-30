#include <sys/elf64.h>
#include <sys/system.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/task_pool.h>
#include <sys/fs.h>
#include <sys/pmap.h>
#include <sys/schedule.h>

Elf64_Ehdr* get_ehdr(struct file *filep); 
Elf64_Phdr* get_phdr(Elf64_Ehdr *ehdr);
Elf64_Shdr* get_shdr(Elf64_Ehdr *ehdr);

int check_ehdr(Elf64_Ehdr *ehdr);
int get_shdr_info(Elf64_Ehdr *ehdr);
int get_phdr_info(Elf64_Ehdr *ehdr);

void print_elf(Elf64_Ehdr* ehdr);
void print_shdr(Elf64_Shdr *shdr);
void print_phdr(Elf64_Phdr *phdr);

struct vma_struct *set_vma_struct(uint64_t s_addr, uint64_t e_addr, uint64_t type, uint64_t flag);
struct vma_struct *traverse_vmas(struct vma_struct *ptr);


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
int create_proc_load_elf(struct file *filep, char *argv[])
{      
        /*
        int valid = parse_elf(filep);
        if(valid != 0)
        {
            return -1;
        }

        task_struct *new_task = create_new_task(&thread1, true);
        struct mm_struct *mm = new_task->mm;
       	if (!mm)
	{
		kprintf("task struct is NULL\n");
	}
        //read the curent pml4 for resoting it
        uint64_t pt = cr3_r();

	Elf64_Ehdr *ehdr = get_ehdr(filep);
        uint64_t s_vaddr, e_vaddr, type, max_vaddr; 
        Elf64_Phdr* phdr = (Elf64_Phdr*) ((void*)ehdr + ehdr->e_phoff);
        int size, flag;
        struct vma_struct *end_vma;

        //for each program header [text -> data -> bss -> heap -> stack]
        for (int n = 0; n < ehdr->e_phnum; ++n)
        {
            if ((int)phdr->p_type == 1)
            {
                s_vaddr = phdr->p_vaddr;
                size = phdr->p_memsz;
                e_vaddr = s_vaddr + size; 
                kprintf("code start address: %d and end vaddress: %d", s_vaddr, e_vaddr);
                
                if (phdr->p_flags == 5)
                {
                    kprintf("TEXT section\n");
                    type = 5;
                    flag = 0;
                }
                else if (phdr->p_flags == 6)
                {
                    kprintf("DATA section\n");
                    type = 6;
                    flag = 0;
                }
                else
                {
                    type = -1;
                    flag =  0;
                }
                kprintf("type set to: %d\n", type);
                // allocate a new vma and add to mm_struct
                struct vma_struct *tdb_vma = set_vma_struct(s_vaddr, e_vaddr, phdr->p_type, flag);
                mm->vma_count++;
                mm->total_vm += size;
     
                if(mm->vm)
                {
                    end_vma = traverse_vmas(mm->vm);//append
                    end_vma->next = tdb_vma;
                }
                else
                {
                     mm->vm = tdb_vma; //new
                }  

                //load plm4 from the process
                cr3_w(mm->pml4);

                //kmmap
                kmmap(s_vaddr, size, flag);
                //1. and 2. text and data
                memcpy((void*) s_vaddr, (void*) ehdr + phdr->p_offset, phdr->p_filesz);
                //3. bss
                memset((void *)s_vaddr + phdr->p_filesz, 0, size - phdr->p_filesz);
                
                //restore the saved plm4
                cr3_w(pt); 
                
            }
            phdr++;
        }
        
        //4.allocate heap
        end_vma = traverse_vmas(mm->vm);
        s_vaddr = e_vaddr = ((((max_vaddr - 1) >> 12) + 1) << 12);//??
        kprintf("heap start address %d and end address %d\n", s_vaddr, e_vaddr);
        end_vma->next = set_vma_struct(s_vaddr, e_vaddr, type, flag);
        mm->vma_count++;
        mm->start_brk = s_vaddr;
        mm->end_brk = e_vaddr; 

        //5. allocate stack
        //s_vaddr = 
        //e_vaddr = 
        kprintf("stack start address %d and end address %d", s_vaddr, e_vaddr);
        end_vma =traverse_vmas(mm->vm);
        end_vma->next = set_vma_struct(s_vaddr, e_vaddr, type, flag);
        mm->vma_count++;
        mm->start_stack = s_vaddr;
        cr3_w(mm->pml4);
        //kmmap();
        cr3_w(pt);

        //mm->total_vm  = ; 

        //handle args
        cr3_w(mm->pml4);

        kprintf("stack ptr at\n");
        //mm->start_stack = ;

        cr3_w(pt);

        //schedule process
	schedule(new_task);
        */
	return 0;
}


struct vma_struct *set_vma_struct(uint64_t s_addr, uint64_t e_addr, uint64_t type, uint64_t flag)
{
    struct vma_struct *new_vma = get_vma_struct();
    new_vma->vm_start = s_addr;
    new_vma->vm_end = e_addr;
    new_vma->type = type;
    new_vma->flag = flag;
    new_vma->next = NULL; //adding at the end
    //new_vma->free ??
    return new_vma;
}

struct vma_struct *traverse_vmas(struct vma_struct *ptr)
{
    struct vma_struct *curr = ptr;
    while(curr->next!=NULL)
    {
        curr = curr->next;
    }
    return curr;
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
