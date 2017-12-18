#include <sys/defs.h>
#include <sys/syscall_handler.h>
#include <sys/schedule.h>
#include <sys/kprintf.h>
#include <dirent.h>
#include <sys/kfs.h>
#include <sys/ktime.h>
#include <sys/kpipe.h>
#include <sys/elf64.h>
#include <sys/error.h>
#include <sys/pmap.h>

extern uint64_t ms;


/**
 * Syscalls definitions
 * These functions should not be directly called. Use sys/syscall.c instead.
 */

/** current process (sys/schedule.c) */
extern task_struct *current;

int64_t sys_yield() {
    run_next_task();
    return 0;
}

int64_t sys_test(uint64_t testArg) {
    __asm__ __volatile__(PUSHREGS);
    kprintf("print me. Argument is %d\n", testArg);
    __asm__ __volatile__(POPREGS);
    return 9001;
}

/**
 * Sleep process for given number of milliseconds
 */
int64_t sys_sleep(uint64_t msec) {
    current->sleep_time = ms + msec;
    add_sleeping_task(current);
    return 0;
}

/**
 * Wait for pid to return
 * If pid is -1, then any child
 *
 * options is ignored right now
 */
int64_t sys_wait4(int pid, uint64_t status, int options) {

    // status is supposd to be a pointer, but our syscall can't support that
    int *status_ptr = (int *)status;

    // if no children, return -1
    if (current->num_children == 0) {
        if (status_ptr)
            *status_ptr = -1;
        return -1;
    }

    add_waiting_task(current);
    current->waiting = true;
    current->wait_pid = pid;
     
    if (status_ptr)
        *status_ptr = 0;
    return 0;
}


/**
 * Fork current process
 * Creates new process as a child of the current
 *
 * Do NOT use this directly. Use fork() in syscall.h!
 */
int64_t sys_fork(uint64_t parent_rip) {


    // create child process
    task_struct *child = fork_process(current);

    // get current process RIP based on stack
    // assumes fork() from syscall.h was called
   // __asm__ __volatile__("movq 104(%%rsp), %0":"=r"(parent_rip));


    kprintf("parent_rip: %p\n", parent_rip);

    // schedule new process like any other
    schedule(child, parent_rip);


    // return value of child should be 0
    // parent would return pid
    child->rax = 0;

    // return child PID to the parent
    return child->pid;
}

/**
 * Exit current process
 * Sets current process to not runnable
 * Process will be removed from available_tasks
 */
int64_t sys_exit() {
    current->runnable = false;
    
    // check if parent was waiting
    if (current->parent) {
        if (current->parent->waiting && 
                (current->parent->wait_pid == current->pid ||  // waiting for this child
                 current->parent->wait_pid == -1)) { // waiting for any child
            current->parent->waiting = false;

            // wait() is supposed to return the child's pid
            current->rax = current->pid;
        }
        current->parent->num_children--;
    }

    sys_yield();
    return 0;
}


int64_t sys_open(char *name, int flags)
{

	kprintf("sys open. file name %s and flags %x\n", name, flags);
	uint64_t ret = sysopen(name, flags);
	if (ret >= 0)
		kprintf("sys open. returned %d\n", ret);
	return ret;
}


int64_t sys_close(int fd)
{
	kprintf("sys close. fd %d\n", fd);	
	uint64_t ret = sysclose(fd);
	if (ret >= 0)
		kprintf("sys close. returned %d\n", ret);
	return ret;
}

//addr on success else current one 
int64_t sys_brk(void *addr)
{	
	uint64_t nbrk = (uint64_t)addr;	
	
	mm_struct *mm = current->mm;
	uint64_t curr_brk = mm->brk;



	if (!mm)
		panic("mm struct in sys brk is NULL\n");

    if(nbrk == -1)
	  {
        return curr_brk;
	  }
    else
    {

        if(nbrk > curr_brk)
        {
          mm->brk = nbrk;
          vma_struct* vma = mm->vm;
          if(!vma)  {
            kprintf("vma_struct in sys brk is NULL\n");
            return -1;
          }
/*
          while(vma)
          {

            if(vma->vm_start <= nbrk && nbrk <= vma->vm_end)
            {
              kprintf("sysbrk causes vmas overlap\n");
              return -1;
            }
            vma = vma->next;
          }

          vma = mm->vm;
*/



          while(vma)
          {
            if(vma->type == HEAP)
            {
              if(nbrk > vma->vm_end )
              {
                if(nbrk >= (USER_STACK_TOP - USER_STACK_SIZE))
                {
                  kprintf("heap colliding with stack\n");
                  return -1;
                }
                vma->vm_end = ALIGN_UP(nbrk);
              }
              break;
            } 
            vma = vma->next;
          }

		    //kprintf("sys brk sucessful. returning %x\n", curr_brk);
        
        }
        else
        {
          kprintf("illegal new brk\n");
          return -1;
        }
    }

        return 0;
}


int64_t sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count) 
{
	kprintf("sys getdents. fd is %d dirp %x,  %d", fd, dirp, count);
	uint64_t ret = sysgetdents(fd, dirp, count);	
	if (ret >= 0)
		kprintf("sys getdents. returned %d\n", ret);
    return ret;
}

int64_t sys_read(unsigned int fd, char *buf, size_t count)
{
  if(fd < 0 || fd >= MAX_FD)
  {
    kprintf("invalid fd\n");
    return -1;
  }

  if(count == 0)
  {
    return 0;
  }

  struct file* filep = current->fdarr[fd]; 

  if(!filep) 
  {
    kprintf("no such file object for fd %d\n", fd);
    return -1;
  }

  if(!buf)
  {
    kprintf("buffer invalid\n");
    return -1;
  }

  ssize_t nread;
  while((nread = filep->fop->read(filep, buf, count, filep->offset)) < 0)
  {
    __asm__ volatile ("sti; hlt;");

  } 

  return nread;
}

int64_t sys_write(unsigned int fd, char *buf, size_t count)
{
  if(fd < 0 || fd >= MAX_FD)
  {
    kprintf("invalid fd\n");
    return -1;
  }


  if(count == 0)
  {
    return 0;
  }


  struct file* filep = current->fdarr[fd]; 

  if(!filep) 
  {
    kprintf("no such file object for fd %d\n", fd);
    return -1;
  }

  size_t nwrite = filep->fop->write(filep, buf, count, filep->offset); 

  return nwrite;
}


int64_t sys_pipe(int *pipefd)
{
	kprintf("sys pipe.\n");
	uint64_t ret = syspipe(pipefd);
	if (ret >= 0)
		kprintf("sys pipe. returned %d \n", ret);
	return ret;
}


int64_t sys_execve(char *file, char **argv, char **envp)
{
	kprintf("sys execvpe. file %s\n", file);	
	
	struct task_struct *new_task;
	

	//create a new process
	new_task = create_elf_process(file, argv, envp);
	
	if(new_task)
	{
		
		//kprintf("created a new task\n");
		
		//replace curr process with this new process CAUTION anything else??	
		new_task->parent = current->parent;
		new_task->pid  = current->pid;
		memcpy((void*)current->fdarr, (void*)new_task->fdarr, sizeof((struct task_struct *)0)->fdarr);//Is this needed?
		

		task_struct *prev = current->prev;
		task_struct *next = current->next;
		prev->next = new_task;
		new_task->next = next;
		kprintf("placed new process between prev and next\n");


		//clean up the original process and loads new pml4
		replace_task(current, new_task);		
		kprintf("replaced the original process\n");

		//run next task
		kprintf("call yield and chill\n");
		sys_yield();
		
		panic("sys execve failed.\n");//execve does not return on success
	}

	return -1;//failure
}


/**
 * Supported syscalls
 * Functions defined above
 * Syscall numbers defined in syscalls.h
 * Number indicates how many arguments function requires
 */
functionWithArg syscalls[] = {
    [SYS_yield] {0, sys_yield}, // 24
    [SYS_sleep] {1, sys_sleep}, // 35
    [SYS_fork] {0, sys_fork}, // 57
    [SYS_test] {1, sys_test}, // 50
    [SYS_exit] {0, sys_exit}, // 60
    [SYS_getdents] {3, sys_getdents}, // 78
    [SYS_open] {2, sys_open},
    [SYS_close] {1, sys_close},
    [SYS_read] {3, sys_read},
    [SYS_write] {3, sys_write},
	[SYS_brk] {1, sys_brk},//12
	[SYS_pipe] {1, sys_pipe}, //22
	[SYS_execve] {3, sys_execve},//59
    [SYS_wait4] {3, sys_wait4} // 61
};

/**
 * Syscall handler (int 0x80)
 *
 * Syscall number: rax
 *
 * Params (in order):
 * rbx, rcx, rdx, rsi, rdi, rbp 
 *
 * Return:
 * rax
 */
void syscall_handler(void) {

    uint64_t num, ret;
    functionWithArg callFunc;
    uint64_t arg0, arg1, arg2, arg3, arg4;

    // read syscall number from rax register
    __asm__ __volatile__(
            "movq %%rax, %0;"
            :"=r" (num)
            ::"%r8", "%rcx", "%rdx", "%rsi", "%rdi" // these registers must not change
            );

    __asm__ __volatile__(PUSHREGS);
    if (num != 4) // don't print for write
        kprintf("Performing syscall %d\n", num);
    __asm__ __volatile__(POPREGS);

    // read arguments from registers
    __asm__ __volatile__(
            "movq %%rdi, %0;"
            "movq %%rsi, %1;"
            "movq %%rdx, %2;"
            "movq %%rcx, %3;"
            "movq %%r8, %4;"
            :"=r" (arg0), "=r" (arg1), "=r" (arg2), "=r" (arg3), "=r" (arg4)
            ::"rdi", "rsi", "rdx", "rcx", "r8"
            );

    // get function associated with syscall
    callFunc = syscalls[num];

    // default return
    ret = 0;
    switch (callFunc.count) {
        case 0:
            ret = callFunc.func();
            break;
        case 1:
            ret = callFunc.func(arg0);
            break;
        case 2:
            ret = callFunc.func(arg0, arg1);
            break;
        case 3:
            ret = callFunc.func(arg0, arg1, arg2);
            break;
        case 4:
            ret = callFunc.func(arg0, arg1, arg2, arg3);
            break;
        case 5:
            ret = callFunc.func(arg0, arg1, arg2, arg3, arg4);
            break;
    }
    // store return value into rax register
    __asm__ __volatile__(
            "movq %0, %%rax;"
            ::"r" (ret)
            );
}

/**
 * Unused, calling this messes up rax defeating the purpose
 */
uint64_t get_sys_return() {
    uint64_t ret;
    __asm__ __volatile__(
            "movq %%rax, %0;"
            :"=r" (ret)
            :: "%rax"
            );

    return ret;
}
