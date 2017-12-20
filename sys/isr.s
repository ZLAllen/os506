#
# isr.s: handle isrs and irqs
#
#

.extern isr_handler
.extern syscall_handler

.macro ISR num
.global _isr\num
_isr\num:
    cli      
    pushq $\num     #dummy error code
    pushq $\num   
    jmp irq_common
.endm

.macro ISR_E num
.global _isr\num
_isr\num:
    cli                 
    pushq \num   
    jmp irq_common
.endm

.text

# no error code
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 9
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19
ISR 20
ISR 21
ISR 22
ISR 23
ISR 24
ISR 25
ISR 26
ISR 27
ISR 28
ISR 29
ISR 30
ISR 31
ISR 32
ISR 33

# with error code
ISR_E 8
ISR_E 10
ISR_E 11
ISR_E 12
ISR_E 13
#ISR_E 14



irq_common:
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %rsp
    movq %rsp, %rdi
    call isr_handler
    popq %rsp
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    add $16, %rsp #pop the num byte and err code
    iretq

# basically irq_common, but without rax
# rax used
.global _isr128
  _isr128:
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    call syscall_handler
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    sti
    iretq

.global _isr14
_isr14:
    cli
    pushq $14
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %rsp             
    call isr_handler
    popq %rsp
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx 
    popq %rax
    add $16, %rsp #pop the num byte and err code
    iretq


.global _fork_ret
_fork_ret:
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    sti
    iretq
