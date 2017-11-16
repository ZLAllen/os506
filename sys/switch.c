#include <sys/switch.h>
#include <sys/schedule.h>

task_struct *task1, *task2;
extern task_struct *current;

void thread1()
{
    kprintf("Thread 1\n");
    switch_to(task1, task2);
    kprintf("Back in thread 1\n");
    switch_to(task1, task2);

    kprintf("Back again in thread 1\n");
    switch_to(task1, task2);
    while(1) {}

   // set_tss_rsp((void*)(ALIGN_UP(task2->rsp) - 16));
    __asm__ volatile("retq");
}

void thread2()
{
    kprintf("Thread 2 \n");
    switch_to(task2, task1);
    kprintf("Back in thread 2\n");
    switch_to(task2, task1);
    kprintf("Back again in thread 2\n");

    while(1) {}

   // set_tss_rsp((void*)(ALIGN_UP(task1->rsp) - 16));
   __asm__ volatile("retq");

}



void init_thread()
{
    task1 = create_new_task(&thread1);
    task2 = create_new_task(&thread2);

    /*
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
    task2->pid = 1;*/

    add_task(task2, 1);
    add_task(task1, 1);

    switch_to(task2, task1);

    __asm__ volatile("retq");

}
/* SEE switch_to in schedule instead
void context_switch(task_struct *task1, task_struct *task2)
{
    __asm__ volatile("movq %[next_rsp], %%rsp"::[next_rsp]"m"(task2->rsp));


}*/
