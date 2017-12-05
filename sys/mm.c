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
    uint64_t aligned_s_addr = s_addr;

    if(s_addr % PGSIZE)
    {
        aligned_s_addr = ALIGN_DOWN(s_addr);
    }
   
    kprintf("%p, %p\n", s_addr, aligned_s_addr);

    //allocate pages
    for(int i = 0; i < num_pages; ++i)
    {
        uint64_t physaddr = (uint64_t)get_free_page();
        kprintf("physaddr: %p\n", physaddr);
        //if(physaddr != 0xD000 && size == 0x228)    while(1);
        map_page(physaddr, aligned_s_addr, flags);


        kprintf("%p, %p\n", physaddr, *((uint64_t*)(getPhys(aligned_s_addr))));
        aligned_s_addr += PGSIZE;
    }

}
