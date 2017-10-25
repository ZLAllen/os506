#ifndef _PMAP_H_
#define _PMAP_H_


struct smap_t {
    uint64_t base, length;
    uint32_t type;
}__attribute__((packed)) *smap;



pmap_init(uint32_t *modulep, void *physbase, void *physfree);


#endif
