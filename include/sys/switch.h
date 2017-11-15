#include <sys/defs.h>
#include <sys/task_pool.h>
// process struct
/*
struct task_struct {
    pid_t pid; // unique process ID, starting at 0
    char *kstack; // bottom of kernel stack
	//uint64_t rip; // location of rip register
	uint64_t rsp; // location of rsp register
    struct task_struct* next; // next task
    struct task_struct *prev; // previous task 
    struct task_struct *parent; // parent task
};

typedef struct task_struct task_struct;
*/
void switch_to(task_struct *me, task_struct *next);
void schedule();
void init_thread();
void create_idle_process();
task_struct *create_new_task(void (*main)(void));

