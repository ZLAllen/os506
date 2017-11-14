#ifndef _TASK_POOL_H_
#define _TASK_POOL_H_

enum state{READY, RUNNING, ZOMBIE};


#include <sys/defs.h>

typedef struct task_struct{
    uint32_t state;
    pid_t pid;
    pid_t ppid;
    uint8_t* kstack;
    struct mm_struct* mm;
    struct task_struct* parent;
    struct task_struct* free;
}task_struct;


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
    int vma_no;
    struct mm_struct* free;
}mm_struct;

void reload_task_struct();

void reload_mm_struct();

void reload_vma_struct();

void* get_task_struct();

void* get_mm_struct();

void* get_vma_struct();

void release_task_struct(task_struct* ptr);

void release_mm_struct(mm_struct* ptr);

void release_vma_struct(vma_struct* ptr);

#endif
