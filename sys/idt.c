#include <sys/defs.h>
#define MAX_IDT 256

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
    uint32_t addr;
}__attribute__((packed));

// 256 interrupts
static struct idt_entry idt[MAX_IDT];

// pointer
static struct idtr_t idtr;

extern void _x86_64_asm_lidt(struct idtr_t* idtr); 
static void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);

void init_idt() {
    idtr.limit = sizeof(struct idt_entry)*MAX_IDT-1;
    idtr.addr = (uint64_t)idt;
    _x86_64_asm_lidt(&idtr);
}


static void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags){
    idt[num].offset_1 = base & 0xFFFF;
    idt[num].offset_2 = (base >> 16) & 0xFFFF;
    idt[num].offset_3 = (base >> 32) & 0xFFFFFFFF;
    
    idt[num].sel = sel;
    idt[num].zero = 0;

    idt[num].type_attr = flags;
}
