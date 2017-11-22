#include <sys/switch.h>
#include <sys/schedule.h>

task_struct *task1, *task2, *task3, *task4;

void thread1()
{
    int x = 100;
	kprintf("Thread 1. Variable is %d.\n", x);
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
    run_next_task();
    kprintf("Back in thread 4\n");
    run_next_task();
    kprintf("Back again in thread 4\n");

    while(1) {}

   // set_tss_rsp((void*)(ALIGN_UP(task1->rsp) - 16));
    __asm__ volatile("retq");

}



void init_thread()
{
    task1 = create_new_task(&thread1);
    task2 = create_new_task(&thread2);
    task3 = create_new_task(&thread3);
    task4 = create_new_task(&thread4);

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
	schedule(task1);
    schedule(task2);
    schedule(task3);
    schedule(task4);

    run_next_task();

    __asm__ volatile("retq");

}
/* SEE switch_to in schedule instead
void context_switch(task_struct *task1, task_struct *task2)
{
    __asm__ volatile("movq %[next_rsp], %%rsp"::[next_rsp]"m"(task2->rsp));


}*/
