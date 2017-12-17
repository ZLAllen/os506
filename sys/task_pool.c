#include <sys/task_pool.h>
#include <sys/kmalloc.h>
#include <sys/error.h>
#include <sys/system.h>
#include <sys/kprintf.h>
#include <sys/schedule.h>
// consider each type of struct having their own pools

// to implement: task_struct, mm_struct, vma_struct, file object
// for stack, just allocate a page


void reload_task_struct();
void reload_mm_struct();
void reload_vma_struct();

void reload_file_struct();
void reload_dirent_struct();

task_struct* free_task_struct;
mm_struct* free_mm_struct;
vma_struct* free_vma_struct;

struct file* free_file_struct;
struct linux_dirent* free_dirent_struct; 


// call only when freelist head empty

void reload_file_struct()
{

    void* ptr = kmalloc();

    free_file_struct = (struct file*)ptr;
    

    struct file* prev = free_file_struct;

    uint64_t num = (uint64_t)ptr + sizeof(struct file);
    prev = free_file_struct;


    for(; num + sizeof(struct file) < (uint64_t)ptr + PGSIZE; num += sizeof(struct file))
    {
        prev->free = (struct file*)num;
        prev = (struct file*)num;
    }

    prev->free = 0;

}

void reload_dirent_struct()
{

    void* ptr = kmalloc();

    free_dirent_struct = (struct linux_dirent*)ptr;
    

    struct linux_dirent* prev = free_dirent_struct;

    uint64_t num = (uint64_t)ptr + sizeof(struct linux_dirent);
    prev = free_dirent_struct;


    for(; num + sizeof(struct linux_dirent) < (uint64_t)ptr + PGSIZE; num += sizeof(struct linux_dirent))
    {
        prev->free = (struct linux_dirent*)num;
        prev = (struct linux_dirent*)num;
    }

    prev->free = 0;

}

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

void* get_file_struct()
{
    if(!free_file_struct)
        reload_file_struct();

    if(!free_file_struct)
        panic("file_struct alloc failed");

    struct file* ret = free_file_struct;
    free_file_struct = free_file_struct->free;

    memset(ret, 0, sizeof(struct file));

    return (void*)ret;
}

void* get_dirent_struct()
{
    if(!free_dirent_struct)
        reload_dirent_struct();

    if(!free_dirent_struct)
        panic("dirent_struct alloc failed");

    struct linux_dirent* ret = free_dirent_struct;
    free_dirent_struct = free_dirent_struct->free;

    memset(ret, 0, sizeof(struct linux_dirent));

    return (void*)ret;
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


void release_file_struct(struct file* ptr)
{
    memset(ptr, 0, sizeof(struct file));
    ptr->free = free_file_struct;
    free_file_struct = ptr;
}


void release_dirent_struct(struct linux_dirent* ptr)
{
    memset(ptr, 0, sizeof(struct linux_dirent));
    ptr->free = free_dirent_struct;
    free_dirent_struct = ptr;
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

int get_free_fd()
{	
	for(int n =0; n < MAX_FD; n++)
	{
		if(current->fdarr[n] == NULL)//yey found a valid fd
			return n;//success
	}

	return -1;//failure
}


void replace_task(struct task_struct *task, struct task_struct *new_task)
{

	struct mm_struct *mm = task->mm;

	//empty the vmas
	struct vma_struct *vma  = mm->vm;
	struct vma_struct *prev_vma = NULL;
    while(vma) 
	{

        prev_vma = vma; //need ref to the last vma in the list
        vma = vma->next;
    }

    //add empty vmas to vma_free_list
    if (prev_vma) 
	{
        prev_vma->next = free_vma_struct;
        free_vma_struct = mm->vm;
    }


	//load new pagetable to clean the exisitng tables
	cr3_w(new_task->mm->pml4);

	//empty the mm_struct	
	mm->vm = NULL;

	
	//empty the task
	memset((void*)task->kstack, 0, KSTACK_SIZE);
	memset((void*)task->fdarr, 0, 8 * MAX_FD);
	task->parent = NULL;
	task->prev = NULL;
	task->next = NULL;

}

