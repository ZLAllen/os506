//#include <stdlib.h>
//#include <stdio.h>
//#include <syscalls.h>
#include <stdio.h>
struct meta {
    size_t size;
    struct meta* next;
    int free;
};

static void* pbreak;
static struct meta* head;


void* brk(void* addr){
    void* ret;

    __asm
        ("syscall"
         :"=a"(ret)
         :"0"(12), "D"(addr)
         :"cc", "rcx", "r11", "memory"
        );
    return ret;

}

void* sbrk(size_t inc){
    void* p, *prev;

    // if pbreak is not initialized
    if(pbreak == (void*)0){
        //make a failed brk call to get current program break
        pbreak = brk((void*)(-1));
    }

    if(!inc)
        return pbreak;

    p = pbreak + inc;
    if(brk(p) == pbreak)
        return (void*)(-1);

    prev = pbreak;
    pbreak = p;

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
           printf("take from freelist\n");
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
    return (struct meta*)p + 1;


}

void free(void* ptr){
    struct meta* bloc;

    if(ptr){
        bloc = (struct meta*)ptr - 1;
        printf("free address is %p\n", bloc);
        if(bloc->free){
           // exit(1);i
           printf("already free %p\n", ptr);
           return;
        }
        bloc->free = 1;
    }

}


int main(){
    char *p, *q;
    p = malloc(16);

    printf("%p\n", p);


    printf("%p\n", head);
    
    q = malloc(16);


    printf("%p\n", q);
   
    printf("%p\n", head->next);
    free(p);
    free(q);

    return 0;

}





