#include <sys/isr.h>
#include <sys/kprintf.h>
#include <sys/system.h>
#include <sys/ktime.h>


static void printkey();

char* msg[20] = {
    "Divison By Zero Exeception",
    "Debug Exception",
    "Non Maskable Interrupt Exception",
    "Breakpoint Exception",
    "Into Detected Overflow Exception",
    "Out of Bounds Exception",
    "Invalid Opcode Exception",
    "No Coprocessor Exception",
    "Double Fault Exception",
    "Coprocessor Segment Overrun Exception",
    "Bad TSS Exception",
    "Segment Not Present Exception",
    "Stack Fault Exception",
    "General Protection Fault Exception",
    "Page Fault Exception",
    "Unknown Interrupt Exception",
    "Coprocessor Fault Exception",
    "Alignment Check Exception (486+)",
    "Machine Check Exception (Pentium/586+)",
    "Reserved Exceptions"
};



// an array of interrupt routines
void *irq_func[16] = {
    addTick, 
    printkey,
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

//key code table here

char kbtb[128] =
{
    0, 27, //ESC
    '1', '2', '3', '4', '5', '6','7','8', //9
    '9', '0','-','=','\b', 
    '\t', 
    'q', 'w', 'e', 'r', //19
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, // handle control key in code


};



static void printkey(){

}

void isr_handler(struct regs* reg){

    void (*funptr)() = 0;
    uint64_t num = reg->num;
    uint64_t err = reg->err;
   


    if(num < 32){
        if(num > 18){
            kprintf("%s\n", msg[19]);
        }else{
            kprintf("%s\n", msg[num]);
        }

        if(err != num)
            kprintf("Error Code: %d\n", err);

    }else if(num < 48){
        
        funptr = irq_func[num-32];

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

}


