#include <sys/pging.h>
#include <sys/kprintf.h>


void init_pging(uint64_t physfree)
{
    // 4 level page tables
    uint64_t *pml4, *pdpt, *pdt, *pt;
    int i, j;
    uint64_t pde, pte;


    //might need to check paging mode
    

    pml4 = (uint64_t*)physfree;
    pdpt = (uint64_t*)(physfree + PGSIZE);
    pdt = (uint64_t*)(physfree + 2*PGSIZE);
    pt = (uint64_t*)(physfree + 3*PGSIZE);


    // supervisor mode, read/write/present
    pde = (uint64_t)pt|PAGE_RW|PAGE_P;
    pte = (uint64_t)0|PAGE_RW|PAGE_P;


    for(i = 0; i < ENTRIES; ++i)
    {
        //zero out pml4, pdpt first
        pml4[i] = 0;
        pdpt[i] = 0;
        // set pdt entries
        pdt[i] = pde + i*PGSIZE;
        for(j = 0; j < ENTRIES; ++j)
        {
            pt[i*ENTRIES+j] = pte + i*PDE_SIZE + j*PTE_SIZE;
        }
    }

    pml4[PML4E(KERN)] = (uint64_t)pdpt|PAGE_RW|PAGE_P;
    pdpt[PDPE(KERN)] = (uint64_t)pdt|PAGE_RW|PAGE_P;



    kprintf("%x, %x\n", PML4E(KERN), PDPE(KERN));
    kprintf("%x, %x, %x, %x\n", pml4, pdpt, pdt, pt);
    kprintf("%x, %x\n", pdt[511], pt[512]);

    cr3_w((uint64_t)pml4);

    kprintf("switch to new page table successfully\n");

}

