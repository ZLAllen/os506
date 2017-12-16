#include <syscall.h>
#include <sys/task_pool.h>
#include <sys/pmap.h>
#include <sys/kprintf.h>
#include <sys/error.h>




uint64_t sysbrk(struct mm_struct *mm,  uint64_t nbrk) 
{
    uint64_t curr_brk = mm->brk;

    if(nbrk == -1)
        return curr_brk;
    else
    {
        if(nbrk > curr_brk)
            mm->brk = nbrk;

        return curr_brk;
    }
}


/*
   uint64_t sysbrk(struct mm_struct *mm,  uint64_t nbrk) 
   {

   if(!mm)
   panic("mm struct null in brk");//send panic


//get the current end brk
uint64_t curr_brk = mm->brk;

struct vma_struct *vma = mm->vm;

while(vma != NULL)//find the heap vma
{
if(vma->vm_start == vma->vm_end && vma->vm_start == mm->start_brk)//boundary 
break;

if (vma->vm_start <= mm->start_brk && mm->start_brk <= vma->vm_end)//within
break;

vma = vma->next;
}

uint64_t aligned_nbrk = ALIGN_UP(nbrk);//page aligned

if (vma && vma->vm_end <= aligned_nbrk)
{
if(vma->next)
{
if(vma->next->vm_start >= aligned_nbrk)//not overlapping with the next vma
{

vma->vm_end = aligned_nbrk;//set
return nbrk;//success
}
}
}

return curr_brk;//error 
}

*/

