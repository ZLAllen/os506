#include <sys/pmap.h>
#include <sys/kprintf.h>


struct freelist_entry{
    uint64_t base;
    uint64_t next;
};

struct freelist_entry* freelist_head;
uint32_t free_pg_count;
uint32_t entry_count;
struct freelist_entry* list_arr;  //descriptr array



void pmap_init(uint32_t *modulep, void *physbase, void *physfree)
{
    // push all available smap_t pointers to an array, send the array to pmap init function

    uint64_t smap_arr[8];
    int num = 0, i;
    uint32_t low_mem, ext_mem;
    uint64_t low_base, ext_base, util_base; // we might share util_base for both page table and descriptors
    struct freelist_entry* prev;

    while(modulep[0] != 0x9001) modulep += modulep[1]+2;

    for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) 
    {
    
        if (smap->type == 1  && smap->length != 0) 
        {
            kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
            if(num < 8)
            {
                smap_arr[num] = (uint64_t)smap;
                num++;
            }
            
        }
    }

    // skip the first 640KB
    // In qemu we will extract two mems, but in 106 machines we wish to have more free pages
    // we should know that the first block is 640KB, the second block we start after physfree
    // we might extract other blocks for future use

    // 640 KB
    smap = (struct smap_t*)smap_arr[0];
    low_mem = (smap->length)/PGSIZE;
    low_base = smap->base;

    // util_base to above kernel
    smap = (struct smap_t*)smap_arr[1];
    util_base = smap->base;

    ext_mem = (smap->length - 0x100000)/PGSIZE; // subtract 1MB
    ext_base = (uint64_t)physfree; // we start from physfree, if it does not work out, we will see

    kprintf("low mem block %d, low mem base %p, util base %p, ext mem block %d, ext mem base %p\n", 
            low_mem, low_base, util_base, ext_mem, ext_base);


    entry_count = low_mem + ext_mem;
    free_pg_count = entry_count;
    

    // generate the head
    freelist_head = (struct freelist_entry*)util_base;
    list_arr = freelist_head;  // keep a reference point of the beginning of descriptors
    freelist_head->base = low_base;
    freelist_head->next = 0;

    prev = freelist_head;

    // span over 640 KB
    for(i = 1; i < low_mem; ++i)
    {
        list_arr[i].base = low_base + i*PGSIZE;
        prev->next = (uint64_t)&list_arr[i];
        prev = (struct freelist_entry*)&list_arr[i];
    }

    // span over high mem
    for(; i < entry_count; ++i)
    {
        list_arr[i].base = ext_base + (i-low_mem)*PGSIZE;
        prev->next = (uint64_t)&list_arr[i];
        prev = (struct freelist_entry*)&list_arr[i];
    }

    // set the tail
    prev->next = 0;

    for(i = low_mem; i < low_mem + 20; ++i) 
    {
        kprintf("entry %d with base %p\n", i, list_arr[i].base);
    }
    
}

