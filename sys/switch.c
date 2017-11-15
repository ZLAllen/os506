#include <sys/switch.h>

task_struct *task1, *task2;

void thread1()
{
    kprintf("Thread 1\n");
    context_switch(task1, task2);

   // set_tss_rsp((void*)(ALIGN_UP(task2->rsp) - 16));
    __asm__ volatile("retq");
}

void thread2()
{
    kprintf("Thread 2\n");
    context_switch(task2, task1);

   // set_tss_rsp((void*)(ALIGN_UP(task1->rsp) - 16));
    __asm__ volatile("retq");

}



void init_thread()
{
    task1 = get_task_struct();
    task2 = get_task_struct();

    task1->kstack = kmalloc();
    task2->kstack = kmalloc();


    //rsp
    task1->kstack[KSTACK_SIZE-2] = (uint64_t)&thread1;

    task1->rsp = (uint64_t)&(task1->kstack[KSTACK_SIZE-2]);

    task1->pid = 0;


    task2->kstack[KSTACK_SIZE-2] = (uint64_t)&thread2;

    task2->rsp = (uint64_t)&(task2->kstack[KSTACK_SIZE-2]);

    task2->pid = 1;

    context_switch(task2, task1);

    __asm__ volatile("retq");

}

void context_switch(task_struct *task1, task_struct *task2)
{
    __asm__ volatile("movq %[next_rsp], %%rsp"::[next_rsp]"m"(task2->rsp));


}
