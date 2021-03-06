#include <sys/pging.h>
#include <sys/kprintf.h>
#include <sys/pmap.h>
#include <sys/kmalloc.h>
#include <sys/task_pool.h>

#define PMLE_REF 0xFFFFFF7FBFDFE000UL  //think of it as recursively visiting pml4, then use page offset to locate pmle 
#define PDPE_REF 0xFFFFFF7FBFC00000UL
#define PDE_REF  0xFFFFFF7F80000000UL
#define PTE_REF  0xFFFFFF0000000000UL

#define VIDEO 0xFFFFFFFF800B8000UL

uint64_t* init_pml4;  //Virtual address reference

uint64_t* pml4;


void* get_kern_pml4()
{
    return pml4;
}

void init_pging(uint64_t physfree)
{
    // 4 level page tables
    uint64_t  *pdpt, *pdt, *pt;


    //might need to check paging mode
    

    pml4 = (uint64_t*)(physfree-PGSIZE*4);

    init_pml4 = (uint64_t*)VADDR(pml4);


    //self reference
    pml4[510] = (uint64_t)pml4|PAGE_RW|PAGE_P;

    //for kernel

    size_t kern_size = (physfree - 0x200000)/PGSIZE;
    uint64_t vaddr = KERNBASE;
    uint16_t pmle_off = vaddr >> 39 & 0x1FF;
    uint16_t pdpe_off = vaddr >> 30 & 0x1FF;
    uint16_t pde_off = vaddr >> 21 & 0x1FF;



    // must change back to kernel only after
    pdpt = (uint64_t*)(physfree - PGSIZE*3);
    pml4[pmle_off] = (uint64_t)pdpt|PAGE_RW|PAGE_P;


    pdt = (uint64_t*)(physfree - PGSIZE*2);
    pdpt[pdpe_off] = (uint64_t)pdt|PAGE_RW|PAGE_P;


    //this page is for page table 2M-4M
    pt = (uint64_t*)(physfree - PGSIZE);
    pdt[pde_off] = (uint64_t)pt|PAGE_RW|PAGE_P;


    int i;
    uint64_t kern = 0x200000;

    // don't think kernel will exceed 2M in this project
    for(i = 0; i < kern_size; ++i)
    {
        pt[i] = kern|PAGE_RW|PAGE_P;
        kern += PGSIZE;
    }



    cr3_w((uint64_t)pml4);

    uint64_t temp = ALIGN_UP((KERN + physfree)) + PGSIZE;
    set_kern_temp_addr((void*)temp);
    set_kern_free_addr((void*)(temp + PGSIZE));


    map_page(0xb8000, VIDEO, (uint64_t)0|PAGE_P|PAGE_RW);

}

void* getPhys(uint64_t vaddr)
{
    uint64_t pte = ((vaddr << 16 >> 28 << 3) | PTE_REF);

    return (void*)pte;
}

void* getPDT(uint64_t vaddr)
{
    uint64_t pde = ((vaddr << 16 >> 37 << 3) | PDE_REF);

    return (void*)pde;
}

void* getPDPT(uint64_t vaddr)
{
    uint64_t pdpe = ((vaddr << 16 >> 46 << 3) | PDPE_REF);
    return (void*)pdpe;
}

void* getPMLT(uint64_t vaddr)
{
    uint64_t pmle = ((vaddr << 16 >> 55 << 3) | PMLE_REF);
    return (void*)pmle;
}

void zero_page(uint64_t paddr)
{
    uint64_t* vaddr = get_kern_temp_addr();

    uint64_t* pte = getPhys((uint64_t)vaddr);
    *pte = paddr|PAGE_P|PAGE_RW;
    memsetw(vaddr, 0, PGSIZE/2);
    free_temp();
}

void map_page(uint64_t paddr, uint64_t vaddr, uint64_t flags)
{
    uint64_t *pmle, *pdpe, *pde, *pte;
    uint64_t addr;

    pmle = (uint64_t *)((vaddr << 16 >> 55 << 3) | PMLE_REF);
    pdpe = (uint64_t *)((vaddr << 16 >> 46 << 3) | PDPE_REF);
    pde = (uint64_t *)((vaddr << 16 >> 37 << 3) | PDE_REF);
    pte = (uint64_t *)((vaddr << 16 >> 28 << 3) | PTE_REF);

    /*
       if(paddr == 0xD000)
       {
         kprintf("didn't crash here\n");
         kprintf("%p, %p\n", pmle, *pmle);
       while(1);
       }
       */

    uint64_t high_flags = flags;
    if(flags & PAGE_COW)
    {
      high_flags &= ~(PAGE_COW);
      high_flags |= (PAGE_RW);
    }

    if(!IS_PRESENT(*pmle))
    {
        addr = (uint64_t)get_free_page();
        zero_page(addr);
        *pmle = addr|high_flags;
    }



    if(!IS_PRESENT(*pdpe))
    {
        addr = (uint64_t)get_free_page(); 
        zero_page(addr);
        *pdpe = addr|high_flags;
    }
       

    if(!IS_PRESENT(*pde))
    {

        addr = (uint64_t)get_free_page();
        zero_page(addr);
        *pde = addr|high_flags;

    }

    if(!IS_PRESENT(*pte))
        *pte = paddr|flags;
    else{
        kprintf("address %p has been mapped\n", vaddr);
        release_page((void*)paddr);
    }
      /* 
       */

    if(vaddr == 0x6000000)
    {
      kprintf("%p, %p, %p, %p\n", *pmle, *pdpe, *pde, *pte);
    } 
}
uint64_t alloc_pml4(){
    // when we create a new pml4, the default contains kernel mapping and self reference
    // which is pml4[511] and pml4[510]
    
    uint64_t pml4;

    pml4 = (uint64_t)get_free_page(); // a phys addr

    zero_page(pml4);

    uint64_t* vir_pml4 = get_kern_free_addr();

    set_kern_free_addr(vir_pml4+PGSIZE);

    map_page(pml4, (uint64_t)vir_pml4,(uint64_t)0|PAGE_P|PAGE_RW|PAGE_U);

    vir_pml4[511] = init_pml4[511]; //kernel mapping is shared

    vir_pml4[510] = pml4|PAGE_P|PAGE_RW; //self ref

    kprintf("debug:nv %p, ov %p, np %p\n", vir_pml4, init_pml4, pml4);

    /*
    //debug 
    cr3_w(pml4);

    kprintf("will I work\n");

    char* vir = kmalloc();

    *vir = 'a';

    kprintf("%c\n", *vir);

    while(1);
*/
    return pml4;  //return the physical address of pml4
}

// probably gonna need a page table walk method
//uint64_t walk_pt(uint64_t physAddr)


