#include <sys/schedule.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>

/** current task */
struct task_struct *current; 

/** list of available tasks - schedule() orders this */
struct task_struct *available_tasks;

/** pid counter */
static pid_t pid = 0;

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
}

void schedule() {
	// select next task
	//task_struct *current, *next;
	//switch_to(current, next);

	// testing - move to next task
	available_tasks = available_tasks->next;
}

void add_task(task_struct *new_task, int test) {

	// is this the first task?
	if (available_tasks == NULL) {
		kprintf("First\n");
		available_tasks = new_task;
	} else {
		kprintf("Hello\n");
		// traverse to the end of the list
		task_struct *cursor = available_tasks;
		while (cursor->next != NULL) {
			cursor = cursor->next;
		}

		cursor->next = new_task;
		new_task->prev = cursor;
	}
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

	kprintf("Task created with pid %d\n", new_task->pid);

	return new_task;
}

void push_regs() {
	// eax, ecx, edx, ebx, esp, ebp, esi, edi
	//__asm__ __volatile__("pusha");
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
}

void pop_regs() {
	//__asm__ __volatile__("popa");
	__asm__ __volatile__(
			"popq %r15;" \
			"popq %r14;" \
			"popq %r13;" \
			"popq %r12;" \
			"popq %r11;" \
			"popq %r10;" \
			"popq %r9;"  \
			"popq %r8;"  \
			"popq %rdi; "\
			"popq %rsi; "\
			"popq %rbp; "\
			"popq %rbx; "\
			"popq %rdx; "\
			"popq %rcx; "\
			"popq %rax; "
			);
}
