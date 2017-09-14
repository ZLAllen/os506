#
# isr.s: handle isrs and irqs
#
#

.extern isr_handler
.extern kputs
.extern test

.macro IRQ num
.global _isr\num
_isr\num:
    cli                 
    movq $\num, %rdi
    call irq_handler
    sti
    iretq
.endm

.text

IRQ 0
IRQ 32
IRQ 33
