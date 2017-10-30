#include <sys/pmap.h>
#include <sys/kprintf.h>
#include <sys/system.h>

struct freelist_entry{
    uint64_t base;
    int map_count;
    uint64_t next;
}__attribute__((__packed__));

struct freelist_entry* freelist_head;
uint32_t free_pg_count;
uint32_t entry_count;
struct freelist_entry* list_arr;  //descriptr array
uint64_t real_physfree;

static struct freelist_entry* walk_list(void* ptr);


void pmap_init(uint32_t *modulep, void *physbase, void *physfree)
{
    // push all available smap_t pointers to an array, send the array to pmap init function

    uint64_t smap_arr[8];
    int num = 0, i;
    uint32_t low_mem;
    uint32_t ext_mem;
    uint64_t low_base;
    uint64_t ext_base; // we might share util_base for both page table and descriptors
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
    low_base = smap->base + KERN;
    

    // util_base to above kernel
    smap = (struct smap_t*)smap_arr[1];
    ext_base = smap->base + KERN;
    ext_mem = (smap->length)/PGSIZE; 

    kprintf("low mem block %d, low mem base %p, ext mem block %d, ext mem base %p\n", 
            low_mem, low_base, ext_mem, ext_base);


    entry_count = ext_mem + low_mem;
    free_pg_count = entry_count;

    //memset

   
   // memsetw((void*)(low_base), 0, (low_mem*PGSIZE)/2);
    //memsetw((void*)ext_base, 0, ext_mem*PGSIZE/2);
    
    // clean up space for physical descriptor after physfree
    //memsetw((void*)physfree, 0, (ext_mem)*sizeof(struct freelist_entry)/2);
    

    real_physfree = (uint64_t)physfree + ((entry_count*sizeof(struct freelist_entry)) & ~(PGSIZE-1)) + PGSIZE;

    kprintf("new physfree is %p\n", real_physfree);


    memsetw(physfree, 0, (real_physfree - (uint64_t)physfree)/2);


 

    // generate the head
    freelist_head = (struct freelist_entry*)physfree;
    list_arr = freelist_head;  // keep a reference point of the beginning of descriptors
    freelist_head->base = low_base;
    freelist_head->map_count = -1;
    freelist_head->next = 0;



    prev = freelist_head;

    // span over 640 KB
    for(i = 1; i < low_mem; ++i)
    {
        list_arr[i].base = low_base + i*PGSIZE;
        list_arr[i].map_count = -1;
        prev->next = (uint64_t)&list_arr[i];
        prev = (struct freelist_entry*)&list_arr[i];
    }
    

    // span over high mem
    for(i = low_mem; i < entry_count; ++i)
    {
        list_arr[i].base = ext_base + (i-low_mem)*PGSIZE;
        if(list_arr[i].base >= (uint64_t)physbase && list_arr[i].base < real_physfree)
        {
            list_arr[i].map_count = 0;
            list_arr[i].next = 0;
            free_pg_count--;
            continue;
        }

        list_arr[i].map_count = -1;
        prev->next = (uint64_t)&list_arr[i];
        prev = (struct freelist_entry*)&list_arr[i];
    }

    // set the tail
    prev->next = 0;

    kprintf("entry count %d, free: %d\n", entry_count, free_pg_count);
/*
    struct freelist_entry* hello = walk_list((void*)(physbase-PGSIZE));
    struct freelist_entry* hi = walk_list((void*)real_physfree);
    kprintf("hi: %p, count %d, next %p\n", hi->base, hi->map_count, hi->next);
    kprintf("hello: %p, count %d, next %p\n", hello->base, hello->map_count, hello->next);
    */



/*
    for(i = 0; i < 10; ++i) 
    {
        kprintf("entry %d with base %p\n", i, list_arr[i].base);
    }
*/


}

void* get_free_page()
{
    if(!freelist_head)
    {
        kprintf("panic: out of memory!\n");
        return 0;
    }

    struct freelist_entry* entry = freelist_head;
    entry->map_count = 0;

    freelist_head = (struct freelist_entry*)freelist_head->next;

    return (void*) (entry->base);
}



void release_page(void* ptr)
{

    struct freelist_entry* entry;
    //traverse the list to verify pointer
    entry = walk_list(ptr);
    if(!entry)
    {
        kprintf("panic: freeing invalid page at %p\n", ptr);
        return;
    }
    else
    {
        if(entry->map_count < 0)
        {
            kprintf("panic: freeing unallocated page!\n");
            return;
        }

        entry->map_count -= 1;

        if(entry->map_count < 0)
        {
            // append this page to freelist
            entry->next = (uint64_t)freelist_head;
            freelist_head = entry;
        }

    }
    
    
}

void inc_map_count(void* ptr)
{

    struct freelist_entry* entry;
    //traverse the list to verify pointer
    entry = walk_list(ptr);
    if(!entry)
    {
        kprintf("panic: increment mapping count of invalid page at %p\n", ptr);
        return;
    }

    if(entry->map_count < 0)
    {
        kprintf("panic: increment mapping count of unallocated page!\n");
        return;
    }

    entry->map_count++;

}

static struct freelist_entry* walk_list(void* ptr)
{
    int i;

    for(i = 0; i < entry_count; ++i)
    {
        if(list_arr[i].base == (uint64_t)ptr)
            return &list_arr[i];
    }

    return 0;
}
