#include <sys/pmap.h>
#include <sys/kprintf.h>


struct freelist_entry{
    uint64_t base;
    uint64_t next;
};

struct freelist_entry* freelist_head;

void pmap_init(uint32_t *modulep, void *physbase, void *physfree)
{
    // push all available smap_t pointers to an array, send the array to pmap init function
/*
    uint64_t smap_arr[8];
    int num = 0, i;
    while(modulep[0] != 0x9001) modulep += modulep[1]+2;

    for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) 
    {
    
        if (smap->type == 1  && smap->length != 0) 
        {
            kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
            if(i < 8)
            {
                smap_arr[num] = (uint64_t)smap;
                num++;
            }
            
        }
    }

    // skip the first 640KB
    for(i = 1; i < num; ++i) 
    {
        smap = (struct smap_t*)smap_arr[i];
        //extract mem
        if(smap->length > PGSIZE) 
        {
            uint64_t start = smap->base;
            uint64_t end = smap->base + smap->length;
        }
        
    }


*/

}

