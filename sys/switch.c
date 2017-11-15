#include <sys/task_pool.h>
#include <sys/switch.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>
#include <sys/system.h>
#include <sys/gdt.h>

#define KSTACK_SIZE 512
#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN_UP(x) ALIGN_MASK((x), ((__typeof__(x))(PGSIZE)-1))


void thread1();
void thread2();
void init_thread();
void context_switch(task_struct *task1, task_struct *task2);

task_struct *task1, *task2;

void thread1()
{
    kprintf("Thread 1\n");
    context_switch(task1, task2);

    set_tss_rsp((void*)(ALIGN_UP(task2->rsp) - 16));
    __asm__ volatile("retq");
}

void thread2()
{
    kprintf("Thread 2\n");
    while(1);
    context_switch(task2, task1);
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

    set_tss_rsp((void*)(ALIGN_UP(task1->rsp) - 16));
    __asm__ volatile("retq");

}

void context_switch(task_struct *task1, task_struct *task2)
{
    __asm__ volatile("movq %[next_rsp], %%rsp"::[next_rsp]"m"(task2->rsp));


}

