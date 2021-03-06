#ifndef _PGING_H_
#define _PGING_H_

#include <sys/system.h>

#define ENTRIES 512

#define PAGE_U (1UL << 2) //paging user
#define PAGE_RW (1UL << 1) //paging writable
#define PAGE_P (1UL << 0) //paging present
#define PAGE_COW (1UL << 9) // copy-on-write bit

#define RX_USER (PAGE_P | PAGE_U)
#define RW_USER (PAGE_P | PAGE_U | PAGE_RW)


#define PDE_SIZE (1 << 21)   //2MB
#define PTE_SIZE (1 << 12)   //4KB
#define PDPE_SIZE (1 << 30)  //1GB

#define IS_PRESENT(pg) ((pg) & PAGE_P)
#define IS_COW(pg) ((pg) & PAGE_COW)
#define IS_RW(pg) ((pg) & PAGE_RW)

#define PML4E(va) ((va >> 39) & ((1UL << 9) - 1))
#define PDPE(va) ((va >> 30) & ((1UL << 9) -1))

#define PA_MASK 0x0000FFFFFFFFF000


void init_pging(uint64_t physfree);

void map_page(uint64_t paddr, uint64_t vaddr, uint64_t flags);

void* getPhys(uint64_t vaddr);

void* getPDT(uint64_t vaddr);

void* getPDPT(uint64_t vaddr);

void* getPMLT(uint64_t vaddr);

uint64_t alloc_pml4();

void zero_page(uint64_t paddr);

#endif
