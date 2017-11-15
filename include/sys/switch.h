#include <sys/task_pool.h>
#include <sys/schedule.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>
#include <sys/system.h>
#include <sys/gdt.h>

#define KSTACK_SIZE 512
#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
//#define ALIGN_UP(x) ALIGN_MASK((x), ((__typeof__(x))(PGSIZE)-1))

void thread1();
void thread2();
void init_thread();
void context_switch(task_struct *task1, task_struct *task2);

