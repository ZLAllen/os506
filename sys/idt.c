#include <sys/defs.h>
#include <sys/isr.h>
#include <sys/system.h>
#include <sys/ktime.h>

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
static struct idtr_t idtr = {sizeof(idt), (uint64_t)idt};

void _x86_64_asm_lidt(struct idtr_t* idtr); 
static void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);
static void pic_remap();

void init_idt() {

    // remap pic first
    //
    //
    pic_remap();

    init_ktime();

    //set up IRQ resp
    //32-bit interrupt gate
 
    idt_set_gate(0, (uint64_t)_isr0, 0x08, 0x8E); 
    idt_set_gate(1, (uint64_t)_isr1, 0x08, 0x8E); 
    idt_set_gate(2, (uint64_t)_isr2, 0x08, 0x8E); 
    idt_set_gate(3, (uint64_t)_isr3, 0x08, 0x8E); 
    idt_set_gate(4, (uint64_t)_isr4, 0x08, 0x8E); 
    idt_set_gate(5, (uint64_t)_isr5, 0x08, 0x8E); 
    idt_set_gate(6, (uint64_t)_isr6, 0x08, 0x8E); 
    idt_set_gate(7, (uint64_t)_isr7, 0x08, 0x8E); 
    idt_set_gate(8, (uint64_t)_isr8, 0x08, 0x8E); 
    idt_set_gate(9, (uint64_t)_isr9, 0x08, 0x8E); 
    idt_set_gate(10, (uint64_t)_isr10, 0x08, 0x8E); 
    idt_set_gate(11, (uint64_t)_isr11, 0x08, 0x8E); 
    idt_set_gate(12, (uint64_t)_isr12, 0x08, 0x8E); 
    idt_set_gate(13, (uint64_t)_isr13, 0x08, 0x8E); 
    idt_set_gate(14, (uint64_t)_isr14, 0x08, 0x8E); 
    idt_set_gate(15, (uint64_t)_isr15, 0x08, 0x8E); 
    idt_set_gate(16, (uint64_t)_isr16, 0x08, 0x8E); 
    idt_set_gate(17, (uint64_t)_isr17, 0x08, 0x8E); 
    idt_set_gate(18, (uint64_t)_isr18, 0x08, 0x8E); 
    idt_set_gate(19, (uint64_t)_isr19, 0x08, 0x8E); 
    idt_set_gate(20, (uint64_t)_isr20, 0x08, 0x8E); 
    idt_set_gate(21, (uint64_t)_isr21, 0x08, 0x8E); 
    idt_set_gate(22, (uint64_t)_isr22, 0x08, 0x8E); 
    idt_set_gate(23, (uint64_t)_isr23, 0x08, 0x8E); 
    idt_set_gate(24, (uint64_t)_isr24, 0x08, 0x8E); 
    idt_set_gate(25, (uint64_t)_isr25, 0x08, 0x8E); 
    idt_set_gate(26, (uint64_t)_isr26, 0x08, 0x8E); 
    idt_set_gate(27, (uint64_t)_isr27, 0x08, 0x8E); 
    idt_set_gate(28, (uint64_t)_isr28, 0x08, 0x8E); 
    idt_set_gate(29, (uint64_t)_isr29, 0x08, 0x8E); 
    idt_set_gate(30, (uint64_t)_isr30, 0x08, 0x8E); 
    idt_set_gate(31, (uint64_t)_isr31, 0x08, 0x8E); 
    idt_set_gate(32, (uint64_t)_isr32, 0x08, 0x8E); 
    idt_set_gate(33, (uint64_t)_isr33, 0x08, 0x8E); 

    _x86_64_asm_lidt(&idtr);

    __asm__ volatile("sti");

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
