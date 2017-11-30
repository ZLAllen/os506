#include <sys/defs.h>
#include <sys/task_pool.h>

#define KSTACK_SIZE 512
#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))

void switch_to(task_struct *me, task_struct *next);
void schedule(task_struct *new_task);
task_struct *get_next_task();
void run_next_task();
pid_t get_next_pid();
task_struct *create_new_task(function thread_fn, bool userp);
void switch_to_user_mode(task_struct *next);

