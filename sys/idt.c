#include <sys/defs.h>
#include <sys/isr.h>
#include <sys/system.h>

#define MAX_IDT 256


// PIC init defs
#define ICW_INIT 0x11
#define MS_OFF 0x20
#define SL_OFF 0x28

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2+1)
#define ICW4_8086 0x01



// an IDT entry
struct idt_entry
{
    uint16_t offset_1; //0-15
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_2; //16-31
    uint32_t offset_3; //32-63
    uint32_t zero;
}__attribute__((packed));

// lidt entry point
struct idtr_t
{
    uint16_t limit;
    uint64_t addr;
}__attribute__((packed));

// 256 interrupts
static struct idt_entry idt[MAX_IDT];

// pointer
static struct idtr_t idtr;

void _x86_64_asm_lidt(struct idtr_t* idtr); 
static void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);
static void pic_remap();

void init_idt() {

    // remap pic first
    //
    //
    pic_remap();

    //set up IRQ resp
    //32-bit interrupt gate
 
    idt_set_gate(0, (uint64_t)_isr0, 0x08, 0x8E); 
    idt_set_gate(32, (uint64_t)_isr32, 0x08, 0x8E); 

    // load the table
    idtr.limit = sizeof(struct idt_entry)*MAX_IDT;
    idtr.addr = (uint64_t)idt;
    _x86_64_asm_lidt(&idtr);

}


static void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags){
    idt[num].offset_1 = base & 0xFFFF;
    idt[num].offset_2 = (base >> 16) & 0xFFFF;
    idt[num].offset_3 = (base >> 32) & 0xFFFFFFFF;
    
    idt[num].selector = sel;
    idt[num].zero = 0;

    idt[num].type_attr = flags;
}

//following osdev guide
static void pic_remap(){
    
    outb(PIC1_COMMAND, ICW_INIT);
    outb(PIC2_COMMAND, ICW_INIT);

    outb(PIC1_DATA, MS_OFF);
    outb(PIC2_DATA, SL_OFF);

    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);

    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // since we dont have an old value, just clear the data reg
    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);

}
