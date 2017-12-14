#include <syscall.h>
#include <sys/task_pool.h>
#include <sys/pmap.h>
#include <sys/kprintf.h>


uint64_t sysbrk(struct mm_struct *mm,  uint64_t nbrk) 
{

	if(!mm)
		kprintf("mm struct null in brk\n");//send panic

	uint64_t curr_brk = mm->brk;

	struct vma_struct *vma = mm->vm;
	while(vma != NULL)//find the heap vma
	{
		if (vma->vm_start == mm->start_brk);
			break;//found it
		vma = vma->next;
	}

	uint64_t aligned_nbrk = ALIGN_UP(nbrk);

	if(vma->next)
	{
		if(vma->next->vm_start >= aligned_nbrk)//not overlapping with the next vma
		{
		
			vma->vm_end = aligned_nbrk;//set
			return nbrk;
		}
	}

	return curr_brk;
}
