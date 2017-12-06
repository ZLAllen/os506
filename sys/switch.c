#include <sys/switch.h>
#include <sys/schedule.h>
#include <sys/pging.h>
#include <sys/syscalls.h>
#include <sys/elf64.h>

task_struct *task1, *task2, *task3, *task4, *task5;

void thread1()
{
    int x = 100;
	kprintf("Thread 1. Variable is %d.\n", x);

    //__asm__ volatile("int $14");


    uint64_t sysReturn;
    uint64_t num = SYS_test;
    uint64_t arg0 = 77;
    __asm__ __volatile__
        ("movq %0, %%rax" :: "r" (num));
    __asm__ __volatile__
        ("movq %0, %%rbx" ::"r" (arg0): "%rax");
    __asm__ volatile ("int $0x80"
        : "=r" (sysReturn)
        :: "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"
    );
    kprintf("Syscal SYS_test with arg 77 returns %d\n", sysReturn);
    while(1);
    run_next_task();
    x++;
    kprintf("Back in thread 1. Variable is %d.\n", x);
    run_next_task();
    x++;
    kprintf("Back again in thread 1. Variable is %d.\n", x);
    run_next_task();
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

    while(1) {}

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



void init_thread() {
    //task1 = create_new_task(false);
    //task2 = create_new_task(false);
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
    //schedule(task1, (uint64_t) thread1);
    //schedule(task2,(uint64_t)thread2);
    /*
    schedule(task3,(uint64_t)thread3);
    schedule(task4,(uint64_t)thread4);
    */
    //schedule(task5);
    kprintf("\nelf process\n");
    char *fname = "test";
    //char *argv[] = {"hello", "arg1", "arg2", '\0'};    
    char *argv[] = {0};
    task_struct *new_task = create_elf_process(fname, argv);
   uint64_t* ret = 0;
    //schedule(new_task, (uint64_t) thread1);
    
        set_tss_rsp((void*)&new_task->kstack[KSTACK_SIZE-1]);
        __asm__ __volatile__
                        ("movq $0x23, %%rax;"
                         "movq %%rax,  %%ds;"
                         "movq %%rax,  %%es;"
                         "movq %%rax,  %%fs;"
                         "movq %%rax,  %%gs;"
                         "movq %1, %%rsp;"
                         "movq %%rsp, %0;"
                         :"=r"(ret)
                         : "r"(new_task->rsp - 8)
                         :"memory", "rax");

        kprintf("%p, %p, %p, %p, %p\n", new_task->kstack[511], new_task->kstack[510], new_task->kstack[509],new_task->kstack[508],new_task->kstack[507]);

        kprintf("%x\n", *ret);
        while(1);
        __asm__ __volatile__("iretq");
    //run_next_task();


}
/* SEE switch_to in schedule instead
void context_switch(task_struct *task1, task_struct *task2)
{
    __asm__ volatile("movq %[next_rsp], %%rsp"::[next_rsp]"m"(task2->rsp));


}*/
