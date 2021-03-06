#ifndef _TASK_POOL_H_
#define _TASK_POOL_H_

enum state{READY, RUNNING, ZOMBIE};
enum vma_type{TEXT, DATA, STACK, HEAP, NO_TYPE};
enum vma_perm{X, RW};

#define STACK_TOP_USR  0xF000000000UL
#define STACK_SIZE_USR 0x10000 
#define MAX_FD 50

#define PROC_NAME_LEN 16

#define MAX_PATH_LEN 49

#include <sys/defs.h>
#include <sys/files.h>
#include <dirent.h> 



// process struct
typedef struct task_struct {
    pid_t pid; // unique process ID, starting at 0
    bool userp; // is this a user process?
    bool runnable; // runnable process
    bool waiting; // process waiting
    bool first_run;
    uint64_t entry;
    uint64_t start_ms; // start time in ms
    uint64_t *kstack; // bottom of kernel stack
	//uint64_t rip; // location of rip register
    uint64_t rsp; // location of rsp register
    uint64_t rax; // rax register value (for fork)
    uint64_t sleep_time; // sleep time - clock ms that it is allowed to run
    uint64_t num_children; // number of child processes
    pid_t wait_pid; // waiting for pid, same as pid for any (-1)
    struct mm_struct *mm; // memory descriptor
    struct task_struct *children; // children
    struct task_struct *next; // next task
    struct task_struct *prev; // previous task 
    struct task_struct *parent; // parent task
    struct task_struct *free; //next free task_struct
    char name[PROC_NAME_LEN]; // process name
    struct file *fdarr[MAX_FD];//keeps track of files for this process
	char cwd[MAX_PATH_LEN + 1];	//1 for the null
} task_struct;

typedef struct vma_struct{
    uint64_t vm_start;
    uint64_t vm_end;
    uint64_t type;
    uint64_t flag;
    struct vma_struct* next;
    struct vma_struct* free;
}vma_struct;


typedef struct mm_struct{
    vma_struct* vm;
    int vma_count;
    int mm_count;
    struct mm_struct* next;
    struct mm_struct* free;
    uint64_t pml4;
    uint64_t total_vm;
    uint64_t start_brk, brk, start_stack;   
    uint64_t arg_start, arg_end, env_start, env_end;
    uint64_t entry;
}mm_struct;


void* get_task_struct();

void* get_mm_struct();

void* get_vma_struct();

void* get_file_struct();

void* get_dirent_struct();

void release_task_struct(task_struct* ptr);

void release_mm_struct(mm_struct* ptr);

void release_vma_struct(vma_struct* ptr);

void release_file_struct(struct file* ptr);

void release_dirent_struct(struct linux_dirent* ptr);

extern task_struct *current;

int get_free_fd();

void replace_task(struct task_struct *old_task, struct task_struct *new_task);

#endif
