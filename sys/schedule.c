#include <sys/schedule.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>
#include <sys/pging.h>

extern task_struct *current;

/** list of available tasks - schedule() orders this */
task_struct *available_tasks;

/** pid counter */
static pid_t pid = 0;

/**
 * Switch from me process to next process
 * Me is currently NULL on the first process
 */
void switch_to(
        task_struct *me,
        task_struct *next) {

    if (me != NULL) {
        // save current processes's stack pointer
        __asm__ __volatile__
            ("movq %%rsp, %0"
             :"=r" (me->rsp) // save stack pointer into current task
             :
             : // clobbered registers
            );
    }

    // switch to next task
    __asm__ __volatile__
        ("movq %0, %%rsp"
         : // no output registers
         :"m" (next->rsp) // replace stack pointer with next task
         : // clobbered registers
        );

    cr3_w(next->mm->pml4);

    // schedule "me" prev task
    if (me != NULL) {
        __asm__ __volatile__(PUSHREGS);
        schedule(me);
        __asm__ __volatile__(POPREGS);
    }

    // check if kernel process or user process
    // switch to ring 3 if needed
    /* TODO - this is bad... still debugging
    if (next->userp) {
        switch_to_user_mode(next);
    }

    __asm__ volatile("retq");
    */
}

/**
 * Schedule new task
 * Basic round-robin for now, just adds to the end of the list
 */
void schedule(task_struct *new_task) {
    if (available_tasks == NULL) {
        available_tasks = new_task;
    } else {
        // traverse to the end of the list
        task_struct *cursor = available_tasks;
        while (cursor->next != NULL) {
            cursor = cursor->next;
        }

        cursor->next = new_task;
        new_task->prev = cursor;
        new_task->next = NULL;
    }
}

/** * Get next available runnable task
 * Removes it from the list of available tasks
 */
task_struct *get_next_task() {
    if (available_tasks == NULL) {
        // TODO - idle process
        return NULL;
    } else {
        task_struct *next_struct = available_tasks;
        available_tasks = available_tasks->next;
        return next_struct;
    }
}

/**
 * Run the next available runnable task
 */
void run_next_task() {
    task_struct *prev = current;
    current = get_next_task();
    switch_to(prev, current);
}

/**
 * Get next available PID
 */
pid_t get_next_pid() {
    return pid++;
}

/**
 * Create a new task
 * This does not schedule the task.
 */
task_struct *create_new_task(function thread_fn, bool userp) {
    task_struct *new_task = get_task_struct();
    new_task->kstack = kmalloc();

    // initialize mm_struct
    mm_struct* my_mm = get_mm_struct();
    my_mm->pml4 = alloc_pml4();

    new_task->mm = my_mm;

    // task rsp
    new_task->kstack[RSP_REG] = (uint64_t)thread_fn;
    new_task->rsp = (uint64_t)&(new_task->kstack[RSP_REG]);

    new_task->pid = get_next_pid();
    new_task->userp = userp;

    kprintf("Process PID %d created\n", new_task->pid);

    return new_task;
}

/**
 * Switch to user mode (ring 3)
 */
void switch_to_user_mode(task_struct *next) {

    // SS
    // RSP
    // RFLAGS
    // CS
    // RIP
    next->kstack[SS_REG] = 0x23; // set SS
    next->kstack[CS_REG] = 0x1b; // set CS
    next->kstack[FLAGS_REG] = 0x200202UL; // set RFLAGS
    __asm__ __volatile__("iretq");
    // set ring 3 bit
    // call iretq
}

/**
 * Fork given process
 */
task_struct *fork_process(task_struct *parent) {
    // create child task
    task_struct *child = get_task_struct();

    // copy parent's task info into child
    memcpy(parent, child, sizeof(task_struct));

    // copy parent's mm struct
    memcpy(parent->mm, child->mm, sizeof(mm_struct));

    // copy vma_structs from parent (entire linked list)
    vma_struct *parent_cursor = parent->mm->vm;
    vma_struct *child_cursor = 0;

    while (parent_cursor) {
        vma_struct *new_vma = get_vma_struct();

        // copy parent's vma
        memcpy(parent_cursor, new_vma, sizeof(vma_struct));

        // first vma in list?
        if (child_cursor) {
            child_cursor->next = new_vma;
            child_cursor = child_cursor->next;
        } else {
            child->mm->vm = new_vma;
            child_cursor = child->mm->vm;
        }
    }

    // copy parent's kernel stack
    memcpy(parent->kstack, child->kstack, sizeof(KSTACK_SIZE));

    // child's return should be 0
    // TODO
    child->kstack[RAX_REG] = 0;

    // child shares the same stack pointer
    child->rsp = (uint64_t)&(child->kstack[RSP_REG]);

    // set copy on write flag and readonly for parent's page table
    // child will share these pages for efficiency
    
    // get current process's (parent's) pml4
    uint64_t pt = cr3_r();

    // set child's pml4 to the same
    child->mm->pml4 = pt;

    child->pid = get_next_pid();
    kprintf("Child process PID %d created\n", child->pid);

    return child;
}

