#include <sys/switch.h>
#include <sys/schedule.h>
#include <sys/pging.h>
#include <syscall.h>
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

    uint64_t sysReturn = test(77);
    kprintf("Syscal SYS_test with arg 77 returns %d\n", sysReturn);
    //sleep(5000);
    kprintf("Should be sleeping\n");
    uint64_t forkRet = fork();
    if (forkRet == 0) {
        kprintf("Child\n");
        x++;
        kprintf("Back in thread 1's child. Variable is %d.\n", x);
        //yield();
        x++;
        kprintf("Back again in thread 1's child. Variable is %d.\n", x);
        //yield();
        kprintf("Thread 1's child exiting!!\n");
        exit();
    } else {
        kprintf("Parent\n");
        x++;
        kprintf("Back in thread 1. Variable is %d.\n", x);
        //yield();
        x++;
        kprintf("Back again in thread 1. Variable is %d.\n", x);
        //yield();
        kprintf("Thread 1 exiting!!\n");
        exit();
    }

   // set_tss_rsp((void*)(ALIGN_UP(task2->rsp) - 16));
    //__asm__ volatile("retq");
}

void thread2()
{
    int x = 0;
    x++;
	kprintf("Thread 2. Variable is %d. \n", x);
    yield();
    x++;
    kprintf("Back in thread 2. Variable is %d. \n", x);
    yield();
    x++;
    kprintf("Back again in thread 2. Variable is %d.\n", x);

    yield();

   // set_tss_rsp((void*)(ALIGN_UP(task1->rsp) - 16));
    kprintf("Thread 2 exiting!!\n");
    exit();

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
    exit();

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
    exit();

}

void thread5() {

	kprintf("Thread 5. I am a user thread \n");

    run_next_task();

    while(1) {}

   // set_tss_rsp((void*)(ALIGN_UP(task1->rsp) - 16));
    exit();

}

void thread6(){
   __asm__ volatile ("movq $50, %rax");

  __asm__ volatile ("int $0x80");


   __asm__ volatile ("movq $50, %rax");

  __asm__ volatile ("int $0x80");
  while(1);
}

void init_thread() {

    task1 = create_new_task(false);
    schedule(task1, (uint64_t) thread1);

}

