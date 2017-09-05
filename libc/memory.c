#define size_t (unsigned int)

struct meta {
    size_t size;
    struct meta* next;
    int free;
}


static void* pbreak;

void* sbrk(int inc){

}


    
