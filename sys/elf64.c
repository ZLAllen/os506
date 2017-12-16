#include <sys/elf64.h>
#include <sys/system.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/task_pool.h>
#include <sys/files.h>
#include <sys/pmap.h>
#include <sys/schedule.h>
#include <sys/mm.h>
#include <sys/pging.h>
#include <sys/kstring.h>
#include <fcntl.h>


Elf64_Ehdr* get_ehdr(struct file *filep); 
Elf64_Phdr* get_phdr(Elf64_Ehdr *ehdr);
Elf64_Shdr* get_shdr(Elf64_Ehdr *ehdr);

int get_shdr_info(Elf64_Ehdr *ehdr);
int get_phdr_info(Elf64_Ehdr *ehdr);

void print_elf(Elf64_Ehdr* ehdr);
void print_shdr(Elf64_Shdr *shdr);
void print_phdr(Elf64_Phdr *phdr);

int validate_ehdr(Elf64_Ehdr *ehdr);
struct task_struct *load_elf(Elf64_Ehdr *ehdr, char *argv[]);

struct vma_struct *set_vma(uint64_t s_addr, uint64_t e_addr, uint64_t type, uint64_t flag);
struct vma_struct *traverse_vmas(struct vma_struct *ptr);



struct task_struct *create_elf_process(char *fname, char *argv[])
{
    struct file *filep = tfs_open(fname, O_RDONLY);//??
    kprintf("file object created");
    Elf64_Ehdr *ehdr = get_ehdr(filep);
    int valid = validate_ehdr(ehdr);
    if (valid != 0)
    {
        kprintf("\nfile object not valid\n");
        return NULL;
    }
	kprintf("calling elf loader\n");
    struct task_struct *newtask = load_elf(ehdr, argv);
	kprintf("task scheduled\n");
    return newtask;
}


//METHOD: load txt, data, and bss section into memory and sets the entry point in task 
struct task_struct *load_elf(Elf64_Ehdr *ehdr, char *argv[])
{      

        //read the curent pml4 for restoring it
        uint64_t pt = cr3_r();

        uint64_t s_vaddr, e_vaddr, top_vaddr = 0;//addresses
        uint64_t vma_type, vma_flag;//vm type and flag

        Elf64_Phdr* phdr = (Elf64_Phdr*) ((void*)ehdr + ehdr->e_phoff);
        int  size;
        struct vma_struct *end_vma;

        task_struct *new_task = create_new_task(true);
        struct mm_struct *mm = new_task->mm;

        uint64_t count = 0;
    
        //for each program header [text -> data -> bss -> heap -> stack]
        for (int n = 0; n < ehdr->e_phnum; ++n)
        {
            if ((int)phdr->p_type == 1)//loadable segment
            {
                s_vaddr = phdr->p_vaddr;
                size = phdr->p_memsz;
                e_vaddr = s_vaddr + size;
                kprintf("vma start address %p and end vaddress %p\n", s_vaddr, e_vaddr);
                
                if (phdr->p_flags == 5)
                {
                    kprintf("TEXT section\n");
                    vma_type = TEXT;
                    vma_flag = RX_USER; //(uint64_t)0|PAGE_P|PAGE_U|PAGE_RW;
                }
                else if (phdr->p_flags == 6)
                {
                    kprintf("DATA section\n");
                    vma_type = DATA;
                    vma_flag = RW_USER; //(uint64_t)0|PAGE_P|PAGE_U;
                }
                else
                {
                  kprintf("no type\n");
                    vma_type = NO_TYPE;
                    vma_flag = RW_USER; //(uint64_t)0|PAGE_P|PAGE_U;
                }
                kprintf("vma type %d and flag %d set\n", vma_type, vma_flag);
                // allocate a new vma and add to mm_struct
                struct vma_struct *tdb_vma = set_vma(s_vaddr, e_vaddr, phdr->p_type, X);
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

                // we need to record the top address available for heap and stack allocation
                top_vaddr = (top_vaddr < e_vaddr) ? e_vaddr : top_vaddr;

                kprintf("new pml4 data: %p\n", mm->pml4);

                //load plm4 from the process
                cr3_w(mm->pml4);






                kmmap(s_vaddr, size, vma_flag);//throws error



                kprintf("%p, %p, %p\n", size, phdr->p_filesz, phdr->p_offset);

/*
                if(count > 2)
                {
                  char* a = (char*)s_vaddr;
                  *a = 'a';
                  kprintf("%c\n", *a);
                  kprintf("didn't crash\n");
                  while(1);
                }
                */
                //1. and 2. text and data
                memmove((void*) ehdr + phdr->p_offset, (void*) s_vaddr, phdr->p_filesz);

                kprintf("%x\n", ehdr);

                kprintf("look into text: %x\n", phdr->p_offset);
                kprintf("try this %x\n", ((Elf64_Ehdr*)((uint64_t) ehdr + phdr->p_offset))->e_entry);


                //3. bss
                memset((void *)s_vaddr + phdr->p_filesz, 0, size - phdr->p_filesz);
               
                //restore the saved plm4
                cr3_w(pt);


            }

            count++;
              phdr++;
        }
        //4.allocate heap 
        end_vma = traverse_vmas(mm->vm);
      
        kprintf("max_addr: %p\n", top_vaddr);
        s_vaddr = e_vaddr = ((((top_vaddr - 1) >> 12) + 1) << 12);//4kb will make it 1GB
        kprintf("heap start address %p and end address %p\n", s_vaddr, e_vaddr);
        end_vma->next = set_vma(s_vaddr, e_vaddr, HEAP, RW);
        mm->vma_count++;
        mm->start_brk = s_vaddr;
        mm->brk = e_vaddr; //by increasing brk we allow heap to grow 


        /*
         * Read the code below and fix/add anything if you need
         *
         */

        // need to make sure user_stack_top didn't collide with s_vaddr
        e_vaddr = USER_STACK_TOP;
        s_vaddr = e_vaddr - USER_STACK_SIZE; // this will be the end of stack
        kprintf("stack upper address %x and lower address %x\n", s_vaddr, e_vaddr);

        end_vma = traverse_vmas(mm->vm);
        end_vma->next = set_vma(s_vaddr, e_vaddr, STACK, RW);
        mm->vma_count++;
        mm->total_vm += USER_STACK_SIZE; 
        //reserve space for return address, stack arguments 
        mm->start_stack = e_vaddr - 8; //8 bytes adddr



        mm->entry = (uint64_t)(ehdr->e_entry);
        cr3_w(mm->pml4);
        kmmap(e_vaddr-PGSIZE, PGSIZE, RW_USER);//pagesize for now


        cr3_w(pt); 


        int nargs = 0; //num args; filename at the beginning and null terminated
	if (argv)
        {
            while(argv[nargs])
            	nargs++;
           
        }
        kprintf("\nnargs %d\n", nargs);

        cr3_w(mm->pml4);

        uint64_t *args_ptr[nargs], *ss; 
        kprintf("start stack at %x\n", mm->start_stack);
        ss = (uint64_t*) mm->start_stack; 
        for (int n = nargs-1; n >= 0; n--)
        {
            uint64_t length = kstrlen(argv[n]) + 1;
            ss =  (uint64_t*)((void*)ss - length);
            memmove(argv[n], (char*)ss, length);
            args_ptr[n] = ss;
            kprintf("args_ptr[%d] contains %x\n", n, ss);
        }
        
        // this line of code will fetch the last argument pointer
        for (int n = nargs-1; n >= 0; n--)
        {
            ss--;
            *ss = (uint64_t)args_ptr[n];
            kprintf("value at %x is %x\n", ss, *ss);
        }
        *ss =  (uint64_t)nargs;
        mm->start_stack = (uint64_t)ss;
        kprintf("start stack at %x\n", mm->start_stack);
        cr3_w(pt);
               
        //schedule process
        kprintf("schedule the new task\n");
        schedule(new_task,(uint64_t)(ehdr->e_entry));
        return new_task;
}


struct vma_struct *set_vma(uint64_t s_addr, uint64_t e_addr, uint64_t type, uint64_t flag)
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


int validate_ehdr(Elf64_Ehdr *ehdr)
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
        //kprintf("file type %d phdr offset %s shdr offset %s flags %d\n", ehdr->e_type, ehdr->e_phoff, ehdr->e_shoff, ehdr->e_flags);//we want executable files = 2
	return 0;
}

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
            if ((int)phdr->p_type == 1)
            {
                //kprintf("type %d offset %s vaddr %s filesz %s memsz %s flags %s\n", phdr->p_type, phdr->p_offset, phdr->p_vaddr, phdr->p_filesz, phdr->p_memsz);
            }
            phdr++;
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
            char *p = (char*)((void *) stable + shdr->sh_name); 
            kprintf("section name %s\n", p);
            //kprintf("section type %x offset %x name %s", shdr->sh_type, shdr->sh_offset, p); 
            shdr++;
        }
        return valid;
}


Elf64_Phdr* get_phdr(Elf64_Ehdr *ehdr)
{
	Elf64_Phdr * phdr = (Elf64_Phdr*)((void*)ehdr + ehdr->e_phoff);	
	return phdr;
}


Elf64_Ehdr* get_ehdr(struct file *filep)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)(((char *)filep->data) + sizeof(struct posix_header_ustar));	
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
    
	kprintf("file type %d phdr offset %s shdr offset %s \n", ehdr->e_type, ehdr->e_phoff, ehdr->e_shoff, ehdr->e_flags);//we want executable files = 2

	kprintf("Program Header\n");
	for(int n = 0; n < ehdr->e_phnum; ++n)
	{
		Elf64_Phdr *phdr = get_phdr(ehdr);
                if((int)phdr->p_type == 1) //PT_LOAD loadable section
		    print_phdr(phdr);
                phdr++;
	} 
        
	kprintf("Section Header\n");
	for(int n = 0; n < ehdr->e_shnum; ++n) 
	{
		Elf64_Shdr *shdr = get_shdr(ehdr);
		print_shdr(shdr);
                shdr++;
	}
}


//print program header fields
void print_phdr(Elf64_Phdr *phdr)
{
	kprintf("type %d offset %s vaddr %s filesz %s memsz %s flags %s\n", phdr->p_type, phdr->p_offset, phdr->p_vaddr, phdr->p_filesz, phdr->p_memsz);
	
}

//print section header fields
void print_shdr(Elf64_Shdr *shdr)
{
	kprintf("section type %x and offset %x", shdr->sh_type, shdr->sh_offset); 

}
