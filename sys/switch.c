/*
#include <sys/task_pool.h>
#include <sys/switch.h>
#include <sys/kmalloc.h>


task_struct *task1, *task2;


void init_thread()
{
    task1 = get_task_struct();
    task2 = get_task_struct();






void thread1()
{
    while(1)
    {
        kprintf("Thread 1\n");
        context_switch(task1, task2);
    }
}

void thread2()
{
    while(1)
    {
        kprintf("Thread 2\n");
        context_switch(task1, task2);
    }
}
*/
