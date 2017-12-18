#include <sys/defs.h>
#include <sys/task_pool.h>

#define KSTACK_SIZE 512
#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))

// register locations on stack
#define SS_REG KSTACK_SIZE-1 // stack segment register
#define SP_REG KSTACK_SIZE-2 // stack pointer
#define FLAGS_REG KSTACK_SIZE-3 // flags register
#define CS_REG KSTACK_SIZE-4 // code segment register
#define IP_REG KSTACK_SIZE-5 // instruction pointer

/** current task */
extern task_struct *current; 

/** idle task if nothing to run */
extern task_struct *idle;

void switch_to(task_struct *me, task_struct *next);
void reschedule(task_struct *task);
void schedule(task_struct *new_task, uint64_t e_entry);
task_struct *create_new_task(bool userp);
task_struct *get_next_task();
void add_waiting_task(task_struct *task);
void add_sleeping_task(task_struct *task);
void run_next_task();
pid_t get_next_pid();
void switch_to_user_mode(task_struct *next);
task_struct *fork_process(task_struct *parent);

void create_idle_task();
void idle_task();
