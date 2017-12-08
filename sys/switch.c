#include <sys/switch.h>
#include <sys/schedule.h>
#include <sys/pging.h>
#include <sys/syscall.h>
#include <sys/elf64.h>
#include <sys/pmap.h>
#include <sys/kstring.h>
#include <sys/kmalloc.h>
#include <sys/system.h>

task_struct *task1, *task2, *task3, *task4, *task5;
void thread6();

void thread1()
{
    int x = 100;
	kprintf("Thread 1. Variable is %d.\n", x);

    //__asm__ volatile("int $14");

    uint64_t sysReturn = test(77);
    kprintf("Syscal SYS_test with arg 77 returns %d\n", sysReturn);
    run_next_task();
    /*
    uint64_t forkRet = fork();
    if (forkRet == 0) {
        kprintf("Child\n");
    } else {
        kprintf("Parent\n");
    }
    */
    x++;
    kprintf("Back in thread 1. Variable is %d.\n", x);
    run_next_task();
    x++;
    kprintf("Back again in thread 1. Variable is %d.\n", x);
    run_next_task();
    kprintf("Boo\n");
    while(1) {}

   // set_tss_rsp((void*)(ALIGN_UP(task2->rsp) - 16));
    __asm__ volatile("retq");
}

void thread2()
{
    int x = 0;
    x++;
	kprintf("Thread 2. Variable is %d. \n", x);
    run_next_task();
    x++;
    kprintf("Back in thread 2. Variable is %d. \n", x);
    run_next_task();
    x++;
    kprintf("Back again in thread 2. Variable is %d.\n", x);
    kprintf("Thread 2 will now run a while(1) and not call run_next_task().");

    run_next_task();

   // set_tss_rsp((void*)(ALIGN_UP(task1->rsp) - 16));
    __asm__ volatile("retq");

}

void thread3()
{
	kprintf("Thread 3 \n");
    run_next_task();
    kprintf("Back in thread 3\n");
    run_next_task();
    kprintf("Back again in thread 3\n");

    while(1) {}

   // set_tss_rsp((void*)(ALIGN_UP(task1->rsp) - 16));
    __asm__ volatile("retq");

}
void thread4()
{
	kprintf("Thread 4 \n");
    //syscallArg0(SYS_fork);
    //__asm__ volatile ("int $0x80");
    run_next_task();
    kprintf("Back in thread 4\n");
    run_next_task();
    kprintf("Back again in thread 4\n");

    while(1) {}

   // set_tss_rsp((void*)(ALIGN_UP(task1->rsp) - 16));
    __asm__ volatile("retq");

}

void thread5() {

	kprintf("Thread 5. I am a user thread \n");

    run_next_task();

    while(1) {}

   // set_tss_rsp((void*)(ALIGN_UP(task1->rsp) - 16));
    __asm__ volatile("retq");

}

void thread6(){
  __asm__ volatile("cli");
  while(1);
}


void init_thread() {
    task1 = create_new_task(false);
    task2 = create_new_task(false);
    /*
    task3 = create_new_task(false);
    task4 = create_new_task(false);
    //task5 = create_new_task(&thread5, true);
    uint64_t *new_page = kmalloc();
    uint64_t *page_table = getPhys((uint64_t)new_page);
    
    // set user bit
    *page_table |= PAGE_U;

    kprintf("%p\n", *page_table);
    */
    schedule(task1, (uint64_t) thread1);
    //schedule(task2,(uint64_t)thread2);
    run_next_task();
    while(1);
    /*
    schedule(task3,(uint64_t)thread3);
    schedule(task4,(uint64_t)thread4);
    */
    //schedule(task5);
    kprintf("\nelf process\n");
    char *fname = "test2";
    //char *argv[] = {"hello", "arg1", "arg2", '\0'};    
    char *argv[] = {0};
    task_struct *new_task = create_elf_process(fname, argv);
   
    cr3_w(new_task->mm->pml4);
  //uint64_t* ret = 0;
    //schedule(new_task, (uint64_t) thread1);
    
        //set_tss_rsp((void*)&new_task->kstack[KSTACK_SIZE-1]);
       /* __asm__ __volatile__
                        ("movq $0x23, %%rax;"
                         "movq %%rax,  %%ds;"
                         "movq %%rax,  %%es;"
                         "movq %%rax,  %%fs;"
                         "movq %%rax,  %%gs;"
                         
                         :"=r"(ret)
                         : "r"(new_task->rsp)
                         :"memory", "rax");
*/

  uint64_t paddr = (uint64_t)get_free_page();
  uint64_t vaddr = 0x6000000;
 map_page(paddr, vaddr, (uint64_t)0|RX_USER);

 uint64_t* pte = getPhys(vaddr);

 kprintf("pte is %p\n", *pte);

 uint64_t size = kstrlen((char*)thread6);
 kprintf("size: %x\n", size);
 memcpy(thread6, (void*)vaddr, size);

 uint64_t cr0 = cr0_r();

 kprintf("cr0: %x\n", cr0);


  set_tss_rsp((void*)&new_task->kstack[KSTACK_SIZE-1]);

   __asm__ volatile("cli");
        __asm__ __volatile__(
        "movq $0x23, %%rax;"
        "movq %%rax, %%ds;"
        "movq %%rax, %%es;"
        "movq %%rax, %%fs;"
        "movq %%rax, %%gs;"
        "pushq %%rax;"         /* ring3 ss, should be _USER_DS|RPL = 0x23 */
        "movq %%rsp, %%rax;"
        "pushq %%rax;"            /* ring3 rsp change back to %0 after*/
        "pushfq;"              /* ring3 rflags */
        "popq %%rax;"
        "or $0x200, %%rax;"    /* Set the IF flag, for interrupts in ring3 */
        "pushq %%rax;"
        "pushq $0x2B;"         /* ring3 cs, should be _USER64_CS|RPL = 0x2B */
        "pushq $1f;"            /* ring3 rip change back to %1 after */ 
        "xorq %%rax, %%rax;"   /* zero the user registers */
        "xorq %%rbx, %%rbx;"
        "xorq %%rcx, %%rcx;"
        "xorq %%rdx, %%rdx;"
        "xorq %%rbp, %%rbp;"
        "xorq %%rsi, %%rsi;"
        "xorq %%rdi, %%rdi;"
        "xorq %%r8, %%r8;"
        "xorq %%r9, %%r9;"
        "xorq %%r10, %%r10;"
        "xorq %%r11, %%r11;"
        "xorq %%r12, %%r12;"
        "xorq %%r13, %%r13;"
        "xorq %%r14, %%r14;"
        "xorq %%r15, %%r15;"
        "iretq;"
        "1:"
        : /* No output */
        :/* "r"(new_task->mm->start_stack), "r"(vaddr)*/
        :"memory", "rax"
    );

   __asm__ volatile ("movq $50, %rax");

  __asm__ volatile ("int $0x80");


    while(1);
    kprintf("shouldn't reach here\n");
    while(1);
    //    kprintf("%p, %p, %p, %p, %p\n", new_task->kstack[511], new_task->kstack[510], new_task->kstack[509],new_task->kstack[508],new_task->kstack[507]);

    //run_next_task();


}
/* SEE switch_to in schedule instead
void context_switch(task_struct *task1, task_struct *task2)
{
    __asm__ volatile("movq %[next_rsp], %%rsp"::[next_rsp]"m"(task2->rsp));


}*/
