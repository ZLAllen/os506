#include <sys/switch.h>
#include <sys/schedule.h>
#include <sys/pging.h>

task_struct *task1, *task2, *task3, *task4, *task5;

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
    //while(1) {}

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

    //while(1) {}

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

    //while(1) {}

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

    //while(1) {}

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
    task1 = create_new_task(&thread1, false);
    task2 = create_new_task(&thread2, false);
    task3 = create_new_task(&thread3, false);
    task4 = create_new_task(&thread4, false);
    task5 = create_new_task(&thread5, true);

    uint64_t *new_page = kmalloc();
    uint64_t *page_table = getPhys((uint64_t)new_page);
    
    // set user bit
    *page_table |= PAGE_U;

    kprintf("%p\n", *page_table);

	schedule(task1);
    schedule(task2);
    schedule(task3);
    schedule(task4);
    schedule(task5);

    run_next_task();

    __asm__ volatile("retq");

}
/* SEE switch_to in schedule instead
void context_switch(task_struct *task1, task_struct *task2)
{
    __asm__ volatile("movq %[next_rsp], %%rsp"::[next_rsp]"m"(task2->rsp));


}*/
