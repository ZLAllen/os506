#include <sys/schedule.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>
#include <sys/pging.h>
#include <sys/gdt.h>

task_struct *current;
task_struct *idle;

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

        // account for the junk added to stack with the switch method calls
        me->rsp += 40;
    }

    __asm__ __volatile__(PUSHREGS);
    cr3_w(next->mm->pml4);
    __asm__ __volatile__(POPREGS);

    // schedule "me" prev task
    if (me != NULL) {
        __asm__ __volatile__(PUSHREGS);
        reschedule(me);
        __asm__ __volatile__(POPREGS);
    }

    // switch to next task
    __asm__ __volatile__
        ("movq %0, %%rsp"
         : // no output registers
         :"m" (next->rsp) // replace stack pointer with next task
         : // clobbered registers
        );

    // check if kernel process or user process
    // switch to ring 3 if needed
    /* TODO - this is bad... still debugging
    */
    if (next->userp) {
        //switch_to_user_mode(next);
        set_tss_rsp((void*)&next->kstack[KSTACK_SIZE-1]);


        __asm__ volatile("cli");
        __asm__ __volatile__(
                "movq $0x23, %%rax;"
                "movq %%rax, %%ds;"
                "movq %%rax, %%es;"
                "movq %%rax, %%fs;"
                "movq %%rax, %%gs;"
                "pushq %%rax;"         /* ring3 ss, should be _USER_DS|RPL = 0x23 */
                "movq %0, %%rax;"
                "pushq %%rax;"            /* ring3 rsp change back to %0 after*/
                "pushfq;"              /* ring3 rflags */
                "popq %%rax;"
                "or $0x200, %%rax;"    /* Set the IF flag, for interrupts in ring3 */
                "pushq %%rax;"
                "pushq $0x2B;"         /* ring3 cs, should be _USER64_CS|RPL = 0x2B */
                "pushq %1;"            /* ring3 rip change back to %1 after */ 
                "xorq %%rax, %%rax;"   /* zero the user registers */
                "xorq %%rbx, %%rbx;"
                "xorq %%rcx, %%rcx;"
                "xorq %%rdx, %%rdx;"
                "xorq %%rbp, %%rbp;"
                "xorq %%rsi, %%rsi;"
                "xorq %%rdi, %%rdi;"
                "xorq %%r8, %%r8;"
                "xorq %%r9, %%r9;"
                "xorq %%r10, %%r10;"
                "xorq %%r11, %%r11;"
                "xorq %%r12, %%r12;"
                "xorq %%r13, %%r13;"
                "xorq %%r14, %%r14;"
                "xorq %%r15, %%r15;"
                "iretq;"
                : /* No output */
                : "r"(next->mm->start_stack), "r"(next->mm->entry)
                :"memory", "rax"
                    );
    }

    // rax register for return values (used for fork)
    __asm__ __volatile__("movq %0, %%rax;"::"r" (next->rax));

    __asm__ volatile("retq");
}

/**
 * Schedule existing task
 * Basic round-robin for now, just adds to the end of the list
 *
 * Call schedule() first for new tasks!
 */
void reschedule(task_struct *task) {

    if (available_tasks == NULL) {
        available_tasks = task;
    } else {
        // traverse to the end of the list
        task_struct *cursor = available_tasks;
        while (cursor->next != NULL) {
            cursor = cursor->next;
        }

        cursor->next = task;
        task->prev = cursor;
        task->next = NULL;
    }
}

void schedule(task_struct *new_task, uint64_t e_entry) {

    if(new_task->userp) {
        new_task->kstack[SS_REG] = 0x23; // set SS
        new_task->kstack[CS_REG] = 0x1b; // set CS

        //for kernel process, this is top of k_stack, it is set when process is
        //created
        new_task->kstack[SP_REG] = new_task->mm->start_stack;
    }

    new_task->kstack[FLAGS_REG] = 0x200202UL; // set RFLAGS

    // let this be the place where they return to
    new_task->kstack[IP_REG] = (uint64_t)e_entry;

    new_task->rsp = (uint64_t)&new_task->kstack[IP_REG];

    new_task->rax = new_task->pid;

    kprintf("e_entry: %x\n", e_entry);

    // idle tasks shouldn't get added
    if (new_task->runnable)
        reschedule(new_task);

}

/**
 * Create a new task
 * This does not schedule the task.
 */
task_struct *create_new_task(bool userp) {
    task_struct *new_task = get_task_struct();
    new_task->kstack = kmalloc();

    new_task->runnable = true;

    // initialize mm_struct
    mm_struct* my_mm = get_mm_struct();
    my_mm->pml4 = alloc_pml4();

    new_task->mm = my_mm;

    // task rsp
    // here we initialize a few things assuming a kthread
    // for user tasks, see schedule_new()
    new_task->kstack[SS_REG] = 0x10; // set SS
    new_task->kstack[CS_REG] = 0x08; // set CS
    new_task->kstack[SP_REG] = (uint64_t)&new_task->kstack[KSTACK_SIZE-1];

    new_task->pid = get_next_pid();
    new_task->userp = userp;

    kprintf("Process PID %d created\n", new_task->pid);

    return new_task;
}

/** * Get next available runnable task
 * Removes it from the list of available tasks
 */
task_struct *get_next_task() {
    task_struct *next_struct = available_tasks;

    // only run runnable tasks
    while (next_struct && !next_struct->runnable) {
        next_struct = next_struct->next;
    }

    if (!next_struct) {
        available_tasks = NULL;
        return idle;
    }

    available_tasks = next_struct->next;
    return next_struct;
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

    child->kstack = kmalloc();
    child->runnable = true;
    child->mm = get_mm_struct();

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

    // child shares the same stack pointer
    child->rsp = (uint64_t)&(child->kstack[SP_REG]);

    child->parent = parent;

    // get current process's (parent's) pml4
    uint64_t pt = cr3_r();

    // set child's pml4 to the same
    child->mm->pml4 = pt;

    // to avoid unncessarily copying the entire page table
    // have child use the same one but set as read-only
    // and a copy on write bit. If page is written to, 
    // then copy first (implementation of this TODO)

    // traverse virtual addresses and translate that 
    // to corresponding page entry
    if (child->mm->vm) {
        uint64_t vaddr = child->mm->vm->vm_start;
        while (vaddr < child->mm->vm->vm_end) {
            uint64_t *pte = getPhys(vaddr);

            // set writable bit to 0
            *pte &= ~PAGE_RW;

            // set copy on write bit to 1
            *pte |= PAGE_COW;

            // move to next entry
            vaddr += PGSIZE;
        }
    }

    child->pid = get_next_pid();
    kprintf("Child process PID %d created\n", child->pid);

    return child;
}

/**
 * Create idle task
 */
void create_idle_task() {
    if (!idle) {
        idle = create_new_task(false);
        idle->runnable = false;
        schedule(idle, (uint64_t)idle_task);
        idle->rax = -1;
    }
}

/**
 * Idle task thread
 */
void idle_task() {

    while(1) {
        run_next_task(); // not calling yield to avoid flooding with syscall debug prints
        //kprintf("Idle! See bottom of sys/schedule.c to remove this msg.\n");
    }
}

