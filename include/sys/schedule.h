#include <sys/defs.h>
#include <sys/task_pool.h>

#define KSTACK_SIZE 512
#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))

typedef void (*function)();

void switch_to(task_struct *me, task_struct *next);
void schedule();
void add_task(task_struct *new_task);
pid_t get_next_pid();
task_struct *create_new_task(function thread_fn);

