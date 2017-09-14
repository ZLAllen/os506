#
# isr.s: handle isrs and irqs
#
#

.extern isr_handler
.extern irq_handler

.macro IRQ num
.global _isr\num
_isr\num:
    cli                 
    movq $\num, %rdi
    call irq_handler
    sti
    iretq
.endm

.macro ISR_NE num
.global _isr\num
_isr\num:
    cli                 
    movq $\num, %rdi
    movq $0, %rsi
    call isr_handler
    sti
    iretq
.endm
.text

.macro ISR_E num
.global _isr\num
_isr\num:
    cli                 
    movq $\num, %rdi
    popq %rsi
    call isr_handler
    sti
    iretq
.endm

# no error code
ISR_NE 0
ISR_NE 1
ISR_NE 2
ISR_NE 3
ISR_NE 4
ISR_NE 5
ISR_NE 6
ISR_NE 7
ISR_NE 9
ISR_NE 15
ISR_NE 16
ISR_NE 17
ISR_NE 18
ISR_NE 19
ISR_NE 20
ISR_NE 21
ISR_NE 22
ISR_NE 23
ISR_NE 24
ISR_NE 25
ISR_NE 26
ISR_NE 27
ISR_NE 28
ISR_NE 29
ISR_NE 30
ISR_NE 31

# with error code
ISR_E 8
ISR_E 10
ISR_E 11
ISR_E 12
ISR_E 13
ISR_E 14

# IRQs
IRQ 32
IRQ 33
