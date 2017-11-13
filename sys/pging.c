#include <sys/pging.h>
#include <sys/kprintf.h>
#include <sys/pmap.h>
#include <sys/kmalloc.h>

#define PMLE_REF 0xFFFFFF7FBFDFE000UL
#define PDPE_REF 0xFFFFFF7FBFC00000UL
#define PDE_REF  0xFFFFFF7F80000000UL
#define PTE_REF  0xFFFFFF0000000000UL

#define VIDEO 0xFFFFFFFF800B8000UL

void map_page(uint64_t paddr, uint64_t vaddr);

void init_pging(uint64_t physfree)
{
    // 4 level page tables
    uint64_t *pml4, *pdpt, *pdt, *pt;


    //might need to check paging mode
    

    pml4 = (uint64_t*)(physfree-PGSIZE*4);

    //self reference
    pml4[510] = (uint64_t)pml4|PAGE_RW|PAGE_P;

    //for kernel

    size_t kern_size = (physfree - 0x200000)/PGSIZE;
    uint64_t vaddr = KERNBASE;
    uint16_t pmle_off = vaddr >> 39 & 0x1FF;
    uint16_t pdpe_off = vaddr >> 30 & 0x1FF;
    uint16_t pde_off = vaddr >> 21 & 0x1FF;



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


    map_page(0xb8000, VIDEO);

    // set a top virtual address
    set_kern_free_addr((void*)(KERN + physfree));


    // test for kmalloc
    /*
    char* test_addr = kmalloc();

    uint64_t* phy = getPhys((uint64_t)test_addr);
   
    kprintf("%p, %p\n", phy, *phy);


    memmove("Hello World", test_addr, 11);

    kprintf("%s\n", test_addr);

    kfree(test_addr);


    test_addr = kmalloc();

     phy = getPhys((uint64_t)test_addr);
   
    kprintf("%p, %p\n", phy, *phy);


    memmove("Hello World", test_addr, 11);

    kprintf("%s\n", test_addr);
    

    test_addr = kmalloc();

     phy = getPhys((uint64_t)test_addr);
   
    kprintf("%p, %p\n", phy, *phy);


    memmove("Hello World", test_addr, 11);

    kprintf("%s\n", test_addr);
*/



/*
    kprintf("%x, %x\n", PML4E(KERN), PDPE(KERN));
    kprintf("%x, %x, %x, %x\n", pml4, pdpt, pdt, pt);
    kprintf("%x, %x\n", pdt[511], pt[512]);


    kprintf("cr0: %x, cr3: %x\n", cr0_r(), cr3_r());

*/
}

void* getPhys(uint64_t vaddr)
{
    uint64_t pte = ((vaddr << 16 >> 28 << 3) | PTE_REF);

    return (void*)pte;
}



void map_page(uint64_t paddr, uint64_t vaddr)
{
    uint64_t *pmle, *pdpe, *pde, *pte;
    uint64_t addr;

    pmle = (uint64_t *)((vaddr << 16 >> 55 << 3) | PMLE_REF);
    pdpe = (uint64_t *)((vaddr << 16 >> 46 << 3) | PDPE_REF);
    pde = (uint64_t *)((vaddr << 16 >> 37 << 3) | PDE_REF);
    pte = (uint64_t *)((vaddr << 16 >> 28 << 3) | PTE_REF);


    if(!IS_PRESENT(*pmle))
    {
        addr = (uint64_t)get_free_page();
        *pmle = addr|PAGE_RW|PAGE_P;
    }


    if(!IS_PRESENT(*pdpe))
    {
        addr = (uint64_t)get_free_page();
        *pdpe = addr|PAGE_RW|PAGE_P;
    }
        

    if(!IS_PRESENT(*pde))
    {
        addr = (uint64_t)get_free_page();

        *pde = addr|PAGE_RW|PAGE_P;
    }

    if(!IS_PRESENT(*pte))
        *pte = paddr|PAGE_RW|PAGE_P;
    else{
        kprintf("address %p has been mapped\n", vaddr);
        release_page((void*)paddr);
    }

}
