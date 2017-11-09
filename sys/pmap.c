#include <sys/pmap.h>
#include <sys/kprintf.h>
#include <sys/system.h>

struct freelist_entry{
    uint64_t base;
    uint32_t map_count;
    uint64_t next;
}__attribute__((__packed__));


struct freelist_entry* freelist_head;
uint32_t free_pg_count;
uint32_t entry_count;
struct freelist_entry* list_arr;  //descriptr array

static struct freelist_entry* walk_list(void* ptr);


uint64_t pmap_init(uint32_t *modulep, const void *physbase, const void *physfree)
{
    // push all available smap_t pointers to an array, send the array to pmap init function

    uint64_t smap_arr[8];
    int num = 0, i, j;
    uint32_t size;
    uint64_t base;
    struct freelist_entry* prev;

    memset(smap_arr, 0, sizeof(uint64_t)*8);

    kprintf("size of freelist_entry %d\n", sizeof(struct freelist_entry));

    while(modulep[0] != 0x9001) modulep += modulep[1]+2;

    for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) 
    {
    
        if (smap->type == 1  && smap->length != 0) 
        {
            kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);

            if(smap->length >= PGSIZE*10)
            {
                if(num < 8)
                {
                    smap_arr[num] = (uint64_t)smap;
                    num++;
                }
            }
            
        }
    }

    // skip the first 640KB
    // In qemu we will extract two mems, but in 106 machines we wish to have more free pages
    // we should know that the first block is 640KB, the second block we start after physfree
    // we might extract other blocks for future use

    if(!num) 
    {
        kprintf("no physical memory detected\n");
        return 0;
    }
    
    // count number of physical pages
    for(i = 0; i < num; ++i)
    {
        smap = (struct smap_t*)smap_arr[i];
        size = smap->length/PGSIZE;
        entry_count += size;
    }


    free_pg_count = entry_count;


    //memset

  
    
    //memsetw((void*)(low_base), 0, (low_mem*PGSIZE)/2);
    //memsetw((void*)ext_base, 0, ext_mem*PGSIZE/2);
    
    // clean up space for physical descriptor after physfree
    //memsetw((void*)physfree, 0, (ext_mem)*sizeof(struct freelist_entry)/2);
    

    uint64_t real_physfree = (uint64_t)physfree + ((entry_count*sizeof(struct freelist_entry)) & ~(PGSIZE-1)) + PGSIZE + PGSIZE*4;



    memsetw(physfree, 0, (real_physfree - (uint64_t)physfree)/2);


 

    // generate the head
    freelist_head = (struct freelist_entry*)physfree;
    list_arr = freelist_head;  // keep a reference point of the beginning of descriptors
    prev = freelist_head;




    uint32_t cnt = 0;
    for(i = 0; i < num; ++i)
    {
        smap = (struct smap_t*)smap_arr[i];
        base = smap->base;
        size = smap->length/PGSIZE;

        for(j = cnt; j-cnt < size; ++j)
        {
            list_arr[j].base = base + (j-cnt)*PGSIZE;
            if(i > 0 && j == cnt + 256) 
            {
                kprintf("test: %p, physbase: %p\n", list_arr[j].base, physbase);
            }
            if((list_arr[j].base >= (uint64_t)physbase) && (list_arr[j].base < real_physfree))
            {
                list_arr[j].map_count = 1;
                list_arr[j].next = 0;
                free_pg_count--;
                continue;
            }
            list_arr[j].map_count = 0;
            if(i == 0 && j == 0)
            {
                freelist_head = (struct freelist_entry*)(VADDR(freelist_head));
            }
            else
            {
                prev->next = (uint64_t)(VADDR(&list_arr[j]));
                prev = &list_arr[j];
            }
        }
        cnt += size;
    }

    // set the tail
    prev->next = 0;
    
    // switch to virtual address
    list_arr = (struct freelist_entry*)VADDR(list_arr);

    kprintf("entry count %d, free: %d\n", entry_count, free_pg_count);


/*
    // testing page alloc and free
    char* a = get_free_page();
    kprintf("first page %p, map count %d\n", a, (walk_list(a))->map_count);
    char* b = get_free_page();
    char* c = get_free_page();

    *a = 'a';
    *b = 'b';
    *c = 'c';

    release_page(a);
    release_page(b);
    release_page(c);

    char* d = get_free_page();
    kprintf("%c\n", *d);
    release_page(d + 1);
    kprintf("reference count: %d\n", (walk_list(d))->map_count);
*/
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

    return real_physfree;

}

void* get_free_page()
{

    if(!freelist_head)
    {
        kprintf("panic: out of memory!\n");
        return (void*)-1;
    }

    struct freelist_entry* entry = freelist_head;

    if(entry->map_count != 0) 
    {
        kprintf("panic: free page with nonzero map_count\n");
        return (void*)-1;
    }

    entry->map_count = 1;

    freelist_head = (struct freelist_entry*)freelist_head->next;

    entry->next = 0;

    return (void*)(entry->base);
}

void* get_zero_page()
{
    void *addr = get_free_page();

    if(addr < 0)
    {
        kprintf("error getting free page\n");
        return 0;
    }

    memsetw(addr, 0, PGSIZE/2);

    return addr;
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
        if(entry->map_count <= 0)
        {
            kprintf("panic: freeing unallocated page!\n");
            return;
        }

        entry->map_count -= 1;

        if(entry->map_count == 0)
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

    if(entry->map_count <= 0)
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
        {
            return &list_arr[i];
        }
    }

    return 0;
}
