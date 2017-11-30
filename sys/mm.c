#include <sys/task_pool.h>
#include <sys/system.h>
#include <sys/kprintf.h>


void kmmap(uint64_t s_addr, int size, uint64_t flags)
{
    int num_pages = ((s_addr + size -1) >> 12) - ((s_addr) >> 12) + 1;
    kprintf("num pages %d\n", num_pages);
    //allocate virtual pages and set flags

}
