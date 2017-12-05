#ifndef _PMAP_H_
#define _PMAP_H_

#include <sys/system.h>

struct smap_t {
    uint64_t base, length;
    uint32_t type;
}__attribute__((packed)) *smap;


#define ALIGN_UP(x) ((x & ~(PGSIZE-1)) + PGSIZE)

#define ALIGN_DOWN(x) ((x & ~(PGSIZE-1)))

uint64_t pmap_init(uint32_t *modulep, const void *physbase, const void *physfree);
void* get_free_page();
void release_page(void* ptr);
void inc_map_count(void* ptr);


#endif
