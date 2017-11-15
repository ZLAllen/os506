#include <sys/schedule.h>
#include <sys/kmalloc.h>

struct task_struct *prev;
struct task_struct *current; // current task and head of the task list
static pid_t pid; // pid counter

void switch_to(
    struct task_struct *me,
    struct task_struct *next) {

	// save current processes's stack pointer
    __asm__ __volatile__
        ("movq %%rsp, %0"
         : // no output registers
         :"r" (me->rsp) // save stack pointer into current task
         : // clobbered registers
	);

	prev->rsp = me->rsp;

	// switch to next task
    __asm__ __volatile__
        ("movq %0, %%rsp"
         : // no output registers
         :"D" (next->rsp) // replace stack pointer with next task
         : // clobbered registers
	);

	// check if kernel process or user process
	// switch to ring 3 if needed
}

void schedule() {
	// select next task
	//task_struct *current, *next;
	//switch_to(current, next);

}

void create_idle_process() {
    task_struct *new_task;
    new_task = kmalloc(sizeof(*new_task));

    new_task->pid = pid++;
}
