#include <sys/schedule.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>

/** current task */
task_struct *current; 

/** list of available tasks - schedule() orders this */
task_struct *available_tasks;

/** pid counter */
static pid_t pid = 0;

/*
<<<<<<< HEAD
void switch_to(struct task_struct *me, struct task_struct *next) {
	//push_regs(); -- crashes
	__asm__ __volatile__(
			"pushq %rax; "\
			"pushq %rcx; "\
			"pushq %rdx; "\
			"pushq %rbx; "\
			"pushq %rbp; "\
			"pushq %rsi; "\
			"pushq %rdi; "\
			"pushq %r8;"  \
			"pushq %r9;"  \
			"pushq %r10;" \
			"pushq %r11;" \
			"pushq %r12;" \
			"pushq %r13;" \
			"pushq %r14;" \
			"pushq %r15;" \
			);

	if (me->prev != NULL) {
		// save current processes's stack pointer
		__asm__ __volatile__
			("movq %%rsp, %0"
			 :"=r" (me->rsp) // output to me's rsp
			 : // save stack pointer into current task
			 : // clobbered registers
			);
	}else{
            kprintf("Null Curr\n");
        }

        while(1);

	// set current to next
	current = next;

	// switch to next task
	__asm__ volatile
		("movq %0, %%rsp"
		 : // no output registers
		 :"m" (next->rsp) // replace stack pointer with next task
		 : // clobbered registers
		);


	// check if kernel process or user process
	// switch to ring 3 if needed

	// pop registers back
	if (me->prev != NULL) {
		pop_regs();
	}
        
=======
*/
/**
 * Switch from me process to next process
 * Me is currently NULL on the first process
 */
void switch_to(
        task_struct *me,
        task_struct *next) {

    if (me != NULL) {
        // save current processes's stack pointer
        __asm__ __volatile__
            ("movq %%rsp, %0"
             :"=r" (me->rsp) // save stack pointer into current task
             :
             : // clobbered registers
            );
    }

    // switch to next task
    __asm__ __volatile__
        ("movq %0, %%rsp"
         : // no output registers
         :"m" (next->rsp) // replace stack pointer with next task
         : // clobbered registers
        );

    // schedule "me" prev task
    if (me != NULL) {
        __asm__ __volatile__(PUSHREGS);
        schedule(me);
        __asm__ __volatile__(POPREGS);
    }

    // check if kernel process or user process
    // switch to ring 3 if needed
}

/**
 * Schedule new task
 * Basic round-robin for now, just adds to the end of the list
 */
void schedule(task_struct *new_task) {
    if (available_tasks == NULL) {
        available_tasks = new_task;
    } else {
        // traverse to the end of the list
        task_struct *cursor = available_tasks;
        while (cursor->next != NULL) {
            cursor = cursor->next;
        }

        cursor->next = new_task;
        new_task->prev = cursor;
        new_task->next = NULL;
    }
}

/**
 * Get next available runnable task
 * Removes it from the list of available tasks
 */
task_struct *get_next_task() {
    if (available_tasks == NULL) {
        // TODO - idle process
        return NULL;
    } else {
        task_struct *next_struct = available_tasks;
        available_tasks = available_tasks->next;
        return next_struct;
    }
}

/**
 * Run the next available runnable task
 */
void run_next_task() {
    task_struct *prev = current;
    current = get_next_task();
    switch_to(prev, current);
}

/**
 * Get next available PID
 */
pid_t get_next_pid() {
    return pid++;
}

/**
 * Create a new task
 * This does not schedule the task.
 */
task_struct *create_new_task(function thread_fn) {
    task_struct *new_task = get_task_struct();
    new_task->kstack = kmalloc();

    // task rsp
    new_task->kstack[KSTACK_SIZE-2] = (uint64_t)thread_fn;
    new_task->rsp = (uint64_t)&(new_task->kstack[KSTACK_SIZE-2]);
    new_task->pid = get_next_pid();

    kprintf("Process PID %d created\n", new_task->pid);

    return new_task;
}
