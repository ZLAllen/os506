#include <sys/isr.h>
#include <sys/kprintf.h>
#include <sys/system.h>
#include <sys/ktime.h>

// an array of interrupt routines
void *irq_func[16] = {
    addTick, 
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};



void irq_handler(uint64_t num){
    void (*funptr)() = 0;


    if(num >= 32 && num < 48){
        funptr = irq_func[num-32];
    }

    if(funptr){
       (*funptr)();
    } 

    //acknowledge device about EOI
    // slave target
    if(num >= 40){
        outb(0xA0, 0x20);
    }

    // master
    outb(0x20, 0x20);
}
