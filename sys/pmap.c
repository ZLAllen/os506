#include <pmap.h>


pmap_init(uint32_t *modulep, void *physbase, void *physfree)
{
    // push all available smap_t pointers to an array, send the array to pmap init function

    uint64_t smap_arr[8];
    char i = 0;
    while(modulep[0] != 0x9001) modulep += modulep[1]+2;

    for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    
        if (smap->type == 1  && smap->length != 0) {
            kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
            smap_arr[i] = (uint64_t)smap;
        }
    }

}
