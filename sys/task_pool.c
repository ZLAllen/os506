#include <sys/task_pool.h>
#include <sys/kmalloc.h>
#include <sys/error.h>
#include <sys/system.h>
#include <sys/kprintf.h>

// consider each type of struct having their own pools

// to implement: task_struct, mm_struct, vma_struct, file object
// for stack, just allocate a page


void reload_task_struct();
void reload_mm_struct();
void reload_vma_struct();

task_struct* free_task_struct;
mm_struct* free_mm_struct;
vma_struct* free_vma_struct;

// call only when freelist head empty

void reload_task_struct()
{
    void* ptr = kmalloc();

    free_task_struct = (task_struct*)ptr;
    

    task_struct* prev = free_task_struct;

    uint64_t num = (uint64_t)ptr + sizeof(task_struct);
    prev = free_task_struct;


    for(; num + sizeof(task_struct) < (uint64_t)ptr + PGSIZE; num += sizeof(task_struct))
    {
        prev->free = (task_struct*)num;
        prev = (task_struct*)num;
    }

    prev->free = 0;


}


void reload_mm_struct()
{
    void* ptr = kmalloc();

    free_mm_struct = (mm_struct*)ptr;

    mm_struct* prev = free_mm_struct;

    uint64_t num = (uint64_t)ptr + sizeof(mm_struct);
    prev = free_mm_struct;

    for(; num + sizeof(mm_struct)< (uint64_t)ptr + PGSIZE; num += sizeof(mm_struct))
    {
        prev->free = (mm_struct*)num;
        prev = (mm_struct*)num;
    }

    prev->free = 0;

}


void reload_vma_struct()
{

    void* ptr = kmalloc();

    free_vma_struct = (vma_struct*)ptr;

    vma_struct* prev = free_vma_struct;

    uint64_t num = (uint64_t)ptr + sizeof(vma_struct);
    prev = free_vma_struct;

    for(; num + sizeof(vma_struct) < (uint64_t)ptr + PGSIZE; num += sizeof(vma_struct))
    {
        prev->free = (vma_struct*)num;
        prev = (vma_struct*)num;
    }

    prev->free = 0;

}

void* get_task_struct()
{
    if(!free_task_struct)
        reload_task_struct();

    if(!free_task_struct)
        panic("task_struct alloc failed");

    task_struct* ret = free_task_struct;
    free_task_struct = free_task_struct->free;

    memset(ret, 0, sizeof(task_struct));

    return (void*)ret;
}


void* get_mm_struct()
{
    if(!free_mm_struct)
        reload_mm_struct();

    if(!free_mm_struct)
        panic("mm_struct alloc failed");

    mm_struct* ret = free_mm_struct;
    free_mm_struct = free_mm_struct->free;

    memset(ret, 0, sizeof(mm_struct));
    
    return (void*)ret;
}


void* get_vma_struct()
{
    if(!free_vma_struct)
        reload_vma_struct();

    if(!free_vma_struct)
        panic("vma_struct alloc failed");

    vma_struct* ret = free_vma_struct;
    free_vma_struct = free_vma_struct->free;

    memset(ret, 0, sizeof(vma_struct));

    return (void*)ret;
}

void release_task_struct(task_struct* ptr)
{
    memset(ptr, 0, sizeof(task_struct));
    ptr->free = free_task_struct;
    free_task_struct = ptr;
}


void release_mm_struct(mm_struct* ptr)
{
    memset(ptr, 0, sizeof(mm_struct));
    ptr->free = free_mm_struct;
    free_mm_struct = ptr;
}


void release_vma_struct(vma_struct* ptr)
{
    memset(ptr, 0, sizeof(vma_struct));
    ptr->free = free_vma_struct;
    free_vma_struct = ptr;
}
