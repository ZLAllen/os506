#ifndef _PMAP_H_
#define _PMAP_H_

#include <sys/system.h>

struct smap_t {
    uint64_t base, length;
    uint32_t type;
}__attribute__((packed)) *smap;


#define ALIGN_UP(x) ((x & ~(PGSIZE-1)) + PGSIZE)

void pmap_init(uint32_t *modulep, void *physbase, void *physfree);
void* get_free_page();
void release_page(void* ptr);
void inc_map_count(void* ptr);

#endif
