#ifndef _PMAP_H_
#define _PMAP_H_

#include <sys/defs.h>

struct smap_t {
    uint64_t base, length;
    uint32_t type;
}__attribute__((packed)) *smap;


#define PGSIZE 0x1000

#define ALIGN_UP(x) ((x & ~(PGSIZE-1)) + PGSIZE)

void pmap_init(uint32_t *modulep, void *physbase, void *physfree);


#endif
