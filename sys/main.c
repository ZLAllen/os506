#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/pmap.h>
#include <sys/pging.h>
#include <sys/tarfs.h>
#include <sys/switch.h>
#include <sys/fs.h>
#include <sys/elf64.h>
#include <sys/syscalls.h>


#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;

void start(uint32_t *modulep, void *physbase, void *physfree)
{

    clr();


    //pml4e + pdpt + pdt + pt
    //physfree += (3 + ENTRIES)*PGSIZE + KERN;


    kprintf("old physfree %p\n", physfree);

    uint64_t real_physfree = pmap_init(modulep,physbase, physfree);

    kprintf("new physfree is %p\n", real_physfree);

    kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);


    init_pging((uint64_t)real_physfree);

    /*
    struct file* fs= tfs_open("lib/", 0);

    kprintf("file name %s\n", ((struct posix_header_ustar*)fs->private_data)->name);

    kprintf("file name %s\n", ((get_tfs_next((struct posix_header_ustar*)fs->private_data))->name));
*/
 /*
    //small test
    uint64_t new_pml4 = alloc_pml4();
    cr3_w(new_pml4);
    kprintf("will I work %p\n", new_pml4);
  */
    init_thread();
    /*
    syscallArg1(SYS_test, 77);
    __asm__ volatile ("int $0x80");
    uint64_t sysReturn = get_sys_return();
    kprintf("Syscal SYS_test with arg 77 returns %d\n", sysReturn);
    */
    //
    /*
    kprintf("\nelf process\n");
    char *fname = "test";
    //char *argv[] = {"hello", "arg1", "arg2", '\0'};    
    char *argv[] = {0};
    create_elf_process(fname, argv);
    */
    //__asm__ volatile ("movq $50, %%rax");
    //__asm__ volatile ("movq $50, %rax");
    //__asm__ volatile ("movq $77, %rbx");
    //ahciTest()
    while(1) __asm__ volatile ("hlt");
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used

    register char *temp1, *temp2;
    for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;

    __asm__ volatile (
        "cli;"
        "movq %%rsp, %0;"
        "movq %1, %%rsp;"
        :"=g"(loader_stack)
        :"r"(&initial_stack[INITIAL_STACK_SIZE])
      );
      init_gdt();
      init_idt();
      __asm__ volatile("sti");

      start(
        (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
        (uint64_t*)&physbase,
        (uint64_t*)(uint64_t)loader_stack[4]
      );
      for(
        temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)(KERN + 0xb8000);
        *temp1;
        temp1 += 1, temp2 += 2
      ) *temp2 = *temp1;

      while(1) __asm__ volatile ("hlt");
}

