#include <sys/schedule.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>

/** current task */
task_struct *current; 

/** list of available tasks - schedule() orders this */
task_struct *available_tasks;

/** pid counter */
static pid_t pid = 0;

void switch_to(
    task_struct *me,
    task_struct *next) {

	if (me->prev != NULL) {
		// save current processes's stack pointer
		__asm__ __volatile__
			("movq %%rsp, %0"
			 :"=r" (me->rsp) // save stack pointer into current task
			 :
			 : // clobbered registers
		);
	}

	//prev->rsp = me->rsp;

	// switch to next task
    __asm__ __volatile__
        ("movq %0, %%rsp"
         : // no output registers
         :"m" (next->rsp) // replace stack pointer with next task
         : // clobbered registers
	);

	// set current to next
    current = next;

    // add prev task to list again (mostly just for testing)
	__asm__ __volatile__(PUSHREGS);
    //add_task(me);
	__asm__ __volatile__(POPREGS);

    //kprintf("Test\n");

	// check if kernel process or user process
	// switch to ring 3 if needed
}

void add_task(task_struct *new_task) {

    // is this the first task?
    if (available_tasks == NULL) {
        available_tasks = new_task;
    } else {
        // traverse to the end of the list
        task_struct *cursor = new_task;
        while (cursor->next != NULL) {
            cursor = cursor->next;
        }

        cursor->next = new_task;
        new_task->prev = cursor;
    }
}

void schedule() {
	// select next task
	//task_struct *current, *next;
	//switch_to(current, next);

}

pid_t get_next_pid() {
    return pid++;
}

task_struct *create_new_task(function thread_fn) {
    task_struct *new_task = get_task_struct();
    new_task->kstack = kmalloc();

    // task rsp
    new_task->kstack[KSTACK_SIZE-2] = (uint64_t)thread_fn;
    new_task->rsp = (uint64_t)&(new_task->kstack[KSTACK_SIZE-2]);
    new_task->pid = get_next_pid();

	kprintf("Process PID %d created", new_task->pid);

    return new_task;
 }
