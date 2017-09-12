#
# idt.s
#
#
# Created on: Sept 11, 2017
#

.text


.global _x86_64_asm_lidt
_x86_64_asm_lidt:
    lidt (%rdi)
    retq
    


