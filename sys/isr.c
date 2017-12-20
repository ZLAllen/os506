#include <sys/isr.h>
#include <sys/kprintf.h>
#include <sys/system.h>
#include <sys/ktime.h>
#include <sys/syscall_handler.h>
#include <sys/pging.h>
#include <sys/pmap.h>
#include <sys/schedule.h>
#include <sys/kmalloc.h>
#include <sys/mm.h>

// define ASCII for special keys
#define SHIFT_UP 0xAA
#define SHIFT_DWN 0x2A
#define CTRL_UP 0x9D
#define CTRL_DWN 0x1D


#define ENTER   0x1C
#define BACKSPACE 0x0E
#define TAB 0x0F
#define BLANKSP  0xE
#define BLACK 0x0700
#define KEY_OFFSET 5


static volatile int ENTR = 0; 
static volatile int SHIFT = 0;
static volatile int CTRL = 0;



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

short kbtb[128] =
{
    0, 27, //ESC
    ('!'<< 8)|'1',('@'<<8)|'2', ('#'<<8)|'3',('$'<<8)|'4',('%'<<8)|'5', ('^'<<8)|'6',('&'<<8)|'7',
    ('*'<<8)|'8', //9
    ('('<<8)|'9',(')'<<8)|'0',('_'<<8)|'-',('+'<<8)|'=','\b', //bckspace
    'I', //tab
    'q', 'w', 'e', 'r', //19
    't', 'y', 'u', 'i', 'o', 'p', ('{'<<8)|'[', ('}'<<8)|']', '\n', //enter key
    0, // handle control key in code	
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', (':'<<8)|';',
	('\"'<<8)|'\'', ('~'<<8)|'`', 0, //left shift
	('|'<<8)|'\\', 'z', 'x', 'c', 'v', 'b', 'n', //49
	'm', ('<'<<8)|',', ('>'<<8)|'.', ('?'<<8)|'/', 0, //right shift
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
void printkey(){

	unsigned char scancode;
        unsigned char decode;
	// read input from the kbd data buffer
	scancode = inb(0x60);
	// a key was just released

        decode = scancode&0x7F;
        if(scancode == ENTER || scancode == TAB || scancode == BACKSPACE){
           update_kkbd(kbtb[decode],1);
           return;
        }

        if(!(scancode&0x80)){
            if(scancode == SHIFT_DWN){
                SHIFT = 1;
                return;
            }
        

            if(scancode == CTRL_DWN){
                CTRL = 1;
                return;
            }

             // key was just pressed
            if (SHIFT == 1){// add 128 when shift is down	
                if(kbtb[decode] > 96 && kbtb[decode] < 126)
                    update_kkbd(kbtb[decode]-32, CTRL);
                else
                    update_kkbd(kbtb[decode]>>8, CTRL);
            } 
            else{	
                //kprintf("%s", kbtb[scancode]);
                update_kkbd(kbtb[decode], CTRL);
            }
        }else{

            if(scancode == SHIFT_UP)
                SHIFT = 0;

            if(scancode == CTRL_UP)
                CTRL = 0;
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

        if(num == 14)
        {
          handle_pg_fault(err);
        }

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




void update_kkbd(char key, int ctrl){
    short* loc = (short*)BASE + 80*25 - KEY_OFFSET;
	short* ptr;  

        ptr = loc;
	/*	
	if (ctrl_flag == 1){
        	*ptr++ = BLACK|'^';
        	*ptr = BLACK|key;
	}
	else
	{
		*ptr++ = BLACK|' ';
		*ptr = BLACK|key;
	}

        */

        if(ctrl){
            *ptr++ = BLACK|0x5E;
            if(key > 96 && key < 126)
                key = key - 32;
        }
        //*ptr++ = BLACK|0x0030;
        *ptr++ = BLACK|key;
        *ptr = BLACK|0x20;

        term_putchar(key);


        return;

}


void handle_pg_fault(uint64_t err)
{
  uint64_t fault_addr = cr2_r(); //get the fault address
  int fault = 0;

    kprintf("fault address %p, error code %x\n", fault_addr, err);

  if(fault_addr >= KERNBASE)
  {
    kprintf("fault occured in kernel\n");
    while(1);          // should probably kill the process
  }
  else if(err & 0x01)
  {
    uint64_t* pte = getPhys(fault_addr);  // fault address page table entry
    uint64_t paddr = *pte;
    uint64_t* pde = getPDT(fault_addr);
    uint64_t* pdpe = getPDPT(fault_addr);
    uint64_t* pmle = getPMLT(fault_addr);


    kprintf("pde: %p, pdpe: %p, pmle: %p\n", *pde, *pdpe, *pmle);  
    vma_struct *vma = current->mm->vm;
    while(vma) 
    {

      kprintf("mmap %p, %p\n", vma->vm_start, vma->vm_end);
      vma = vma->next;
    }
      kprintf("pte: %p\n", *pte);
    if(IS_COW(paddr) && !IS_RW(paddr)) 
    {
      // page is cow, need to make copy of it
      uint64_t naddr = (uint64_t)get_free_page();
      
      zero_page(naddr); //clear the page
      kprintf("naddr: %p\n", naddr);

      uint64_t temp_vaddr = (uint64_t)get_kern_temp_addr();
      map_page(naddr, temp_vaddr, PAGE_P|PAGE_U|PAGE_RW);
      memmove((void*)(ALIGN_DOWN(fault_addr)),(void*)temp_vaddr, PGSIZE);
      free_temp();

      *pte = naddr|PAGE_U|PAGE_P|PAGE_RW;
      release_page((void*)(paddr & (~(PGSIZE-1)))); // this is decrement the reference count
    }else
    {
      fault = 1;
    }  
  }else{
    vma_struct *vma = current->mm->vm;
    while(vma) 
    {

      kprintf("mmap %p, %p\n", vma->vm_start, vma->vm_end);
      if(fault_addr >= vma->vm_start && fault_addr < vma->vm_end)
      {
        kmmap(vma->vm_start, vma->vm_end - vma->vm_start, (uint64_t)0|PAGE_U|PAGE_P|PAGE_RW);
        break;
      }
      vma = vma->next;
    } 

    if(!vma) // not found
    {
      fault = 1;
    }
  }

  if(fault)
  {
    panic("Segmentation Fault, process ends\n");
    sys_exit();
  }
  
  invlpg(fault_addr);
}

