#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <stdio.h>

#define MALIGN 64
#define MALIGN_UP(x) (((x)&(~(MALIGN-1))) + MALIGN);

struct meta {
    size_t size;
    struct meta* next;
    int free;
};

static struct meta* head;


void* sbrk(size_t inc){
    char* p, *prev;

    prev = brk((void*)(-1));


    printf("prev is %p\n", prev);

    if(!inc)
        return prev;

    p = prev + inc;
    p = (char*)MALIGN_UP((uint64_t)p);
    //printf("new brk is %p\n", p);
    if((char*)(brk((void*)p)) < 0)
        return (void*)(-1);

    printf("new brk is %p\n", p);
    return prev;
}

// an inefficient implementation of malloc
// possible way to improve: free list rearrange, memory alignment   
void* malloc(size_t size){
    void* p;
    struct meta* ptr, *holder;

    if(size < 0)
        return 0;


    //no memory in the free list, call sbrk to allocate 
    if(!head){
        p = sbrk(size + sizeof(struct meta));
        if(p == (void*)-1)
            return 0;
        //initialized the meta block
        head = (struct meta*)p;
        head->size = size;
        head->next = 0;
        head->free = 0;




        //walk pass the meta block and yield allocated space to user
        return (struct meta*)p + 1;
    }

    // if head is not zero, free list is available, check for free space
   ptr = head;

   // we should probably split the memory here, for fast implementation purpose
   // we will postpone this idea
   while(ptr){
       if((ptr->free) && (ptr->size >= size)){
           ptr->free = 0;
           return ptr+1;
       }
      
       
       if(ptr->next)
           ptr = ptr->next;
       else
           break;
   }

   // did not find a usable memory

    p = sbrk(size + sizeof(struct meta));
    if(p == (void*)-1)
        return 0;

    holder = (struct meta*)p;
    holder->size = size;
    holder->next = 0;
    holder->free = 0;

    ptr->next = holder;

    //walk pass the meta block and yield allocated space to user
    return holder+1;



}

void free(void* ptr){
    struct meta* bloc;

    if(ptr){
        bloc = (struct meta*)ptr - 1;
        if(bloc->free){
           exit(1);
        }
        bloc->free = 1;
    }

}






