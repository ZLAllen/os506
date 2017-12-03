#include <sys/mm.h>
#include <sys/pmap.h>
#include <sys/pging.h>
#include <sys/system.h>
#include <sys/kprintf.h>


void kmmap(uint64_t s_addr, int size, uint64_t flags)
{
    int num_pages = ((s_addr + size -1) >> 12) - ((s_addr) >> 12) + 1;
    kprintf("num pages %d\n", num_pages);


    // make sure s_addr is aligned to a page granurality 
    uint64_t aligned_s_addr = ALIGN_UP(s_addr);
    

    //allocate pages
    for(int i = 0; i < num_pages; ++i)
    {
        uint64_t physaddr = (uint64_t)get_free_page();
        map_page(physaddr, aligned_s_addr, flags);
        aligned_s_addr += PGSIZE;
    }
    

}
