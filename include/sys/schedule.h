#include <sys/defs.h>
#include <sys/task_pool.h>

#define KSTACK_SIZE 512
#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))

/** push all registers */
#define PUSHREGS \
	"pushq %rax; "\
	"pushq %rcx; "\
	"pushq %rdx; "\
	"pushq %rbx; "\
	"pushq %rbp; "\
	"pushq %rsi; "\
	"pushq %rdi; "\
	"pushq %r8;  "\
	"pushq %r9;  "\
	"pushq %r10; "\
	"pushq %r11; "\
	"pushq %r12; "\
	"pushq %r13; "\
	"pushq %r14; "\
	"pushq %r15; "	

#define POPREGS \
	"popq %r15; "\
	"popq %r14; "\
	"popq %r13; "\
	"popq %r12; "\
	"popq %r11; "\
	"popq %r10; "\
	"popq %r9;  "\
	"popq %r8;  "\
	"popq %rdi; "\
	"popq %rsi; "\
	"popq %rbp; "\
	"popq %rbx; "\
	"popq %rdx; "\
	"popq %rcx; "\
	"popq %rax; "

/** pop all registers */

typedef void (*function)();

void switch_to(task_struct *me, task_struct *next);
void schedule(task_struct *new_task);
task_struct *get_next_task();
void run_next_task();
pid_t get_next_pid();
task_struct *create_new_task(function thread_fn, bool userp);
void switch_to_user_mode(task_struct *next);

