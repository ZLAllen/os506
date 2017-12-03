#include <sys/kmalloc.h>
#include <sys/pging.h>
#include <sys/pmap.h>
#include <sys/error.h>
#include <sys/kprintf.h>

void* kern_free_addr;
void* k_freelist_pointer;

void* get_kern_free_addr()
{
    return kern_free_addr;
}


void set_kern_free_addr(void* ptr)
{
    kern_free_addr = ptr;
}

// return a clean page
void* kmalloc()
{
    void* paddr = get_free_page();

    if(paddr < 0) 
    {
        panic("kmalloc: alloc failed");
    }

    // register mem to page table
   map_page((uint64_t)paddr, (uint64_t)kern_free_addr, (uint64_t)0|PAGE_P|PAGE_RW);
 
   void* vaddr = kern_free_addr; 

   uint64_t* pte = getPhys((uint64_t)vaddr);

   kprintf("pte: %p, vaddr: %p\n", *pte, vaddr);

   kern_free_addr += PGSIZE;

   memsetw(vaddr, 0, PGSIZE/2);

   return vaddr; 
}

void kfree(void* vaddr){
    // try to free physical page
    if(vaddr)
    {
        uint64_t* pte = getPhys((uint64_t)vaddr);

        release_page((void*)((*pte)&PA_MASK));

        *pte = 0;

        invlpg((uint64_t)vaddr);
    }
}

