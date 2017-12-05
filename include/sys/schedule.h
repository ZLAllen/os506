#include <sys/defs.h>
#include <sys/task_pool.h>

#define KSTACK_SIZE 512
#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))

// register locations on stack
#define SS_REG KSTACK_SIZE-1 // stack segment register
#define SP_REG KSTACK_SIZE-2 // stack pointer
#define FLAGS_REG KSTACK_SIZE-3 // flags register
#define CS_REG KSTACK_SIZE-4 // code segment register
#define IP_REG KSTACK_SIZE-5 // instruction pointer register
#define RAX_REG KSTACK_SIZE-7 // return addr register

/** current task */
extern task_struct *current; 

void switch_to(task_struct *me, task_struct *next);

void schedule(task_struct *task);
void schedule_new(task_struct *new_task, function task_entry, uint64_t stack_top);
task_struct *create_new_task(function thread_fn, uint64_t stack_top, bool userp);

task_struct *get_next_task();
void run_next_task();

pid_t get_next_pid();
void switch_to_user_mode(task_struct *next);

task_struct *fork_process(task_struct *parent);

