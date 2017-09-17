#include <sys/isr.h>
#include <sys/kprintf.h>
#include <sys/system.h>
#include <sys/ktime.h>

// define ASCII for special keys
#define SHIFT_UP 0xAA
#define SHIFT_DWN 0x2A
#define CTRL_UP 0x9D
#define CTRL_DWN 0x1D

#define ENTER   0x9C
#define BSPACE  0xE

#define BASE 0xb8000
#define BLACK 0x0700
#define KEY_OFFSET 5

volatile int READING = 0;
volatile int ENTR = 0; 
volatile int SHIFT = 0;
volatile int CTRL = 0;

volatile char* cursor;
volatile char* current;

void kprintf(const char *format, ...);


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

//key code table here; scancode is an index in this table

char kbtb[128] =
{
    0, 27, //ESC
    '1', '2', '3', '4', '5', '6','7','8', //9
    '9', '0','-','=','\b', //bckspace
    '\t', //tab
    'q', 'w', 'e', 'r', //19
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', //enter key
    0, // handle control key in code	
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`', 0, //left shift
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', //49
	'm', ',', '.', '/', 0, //right shift
	'*',
	0, //alt 
	' ', //space
    0, //caps
	0, //59 F1 key
	0, 0, 0, 0, 0, 0, 0, 0,
	0, //F10
	0, //69 num lock
	0, //scroll lock
	0, //home
    0, //up arrow
	0, //page up
	'-', 
	0, //left arrow
	0,
	0,  //right arrow
	'+', //
	0, //end key
	0, //down arrow
	0, //page down
	0, //insert 
	0, //delete
	0, 0, 0,
	0, //f11 
	0, //f12
	0, //all the other keys are undefined	

};

//TODO add enter and bspace 
static void printkey(){

	unsigned char scancode;
	// read input from the kbd data buffer
	scancode = inb(0x60);
	// a key was just released
	if (scancode & 0x80){
		switch(scancode) {
			case SHIFT_UP:
				SHIFT = 0;
		    	break;
	    	case SHIFT_DWN:
		    	SHIFT = 1;
				break;
	 		case CTRL_UP:
				CTRL = 0;
				break;
			case CTRL_DWN:
				CTRL = 1;
				break;	
		}
	}
	else{ // key was just pressed
		if (SHIFT == 1){// add 128 when shift is down	
			//kprintf("%s", kbtb[scancode + 0x80]);
			update_kkbd(kbtb[scancode + 0x80]) ;
		}
		else{	
			//kprintf("%s", kbtb[scancode]);
			update_kkbd(kbtb[scancode]);
		}
	}	
}



void isr_handler(struct regs reg){

    void (*funptr)() = 0;
    uint64_t num = reg.num;
    uint64_t err = reg.err;
   


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

void update_kkbd(char key){
        short* loc = (short*)BASE + 80*25 - KEY_OFFSET;
	short* ptr;  

        ptr = loc;

        //*ptr++ = BLACK|0x0030;
        *ptr = BLACK|key;
        return;

}



