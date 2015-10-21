
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#
# isr.s
#

.text

.globl _x86_64_asm_ildt
_x86_64_asm_ildt:
        lidt (%rdi)
        retq

.macro SET_INTERRUPT_EC name num
	.globl \name
	.type \name, @function
	\name:
	     	pushq $0
	    	pushq $\num
		jmp __isrroutine
.ENDM

.macro SET_INTERRUPT name num
        .globl \name
        .type \name, @function
        \name:
                pushq $\num
                jmp __isrroutine
.ENDM


SET_INTERRUPT_EC isr0 0 
SET_INTERRUPT_EC isr1 1 
SET_INTERRUPT_EC isr2 2 
SET_INTERRUPT_EC isr3 3 
SET_INTERRUPT_EC isr4 4 
SET_INTERRUPT_EC isr5 5 
SET_INTERRUPT_EC isr6 6 
SET_INTERRUPT_EC isr7 7 
SET_INTERRUPT_EC isr8 8
SET_INTERRUPT_EC isr9 9
SET_INTERRUPT_EC isr10 10 
SET_INTERRUPT_EC isr11 11
SET_INTERRUPT_EC isr12 12 
SET_INTERRUPT_EC isr13 13 
SET_INTERRUPT isr14 14 
SET_INTERRUPT_EC isr15 15
SET_INTERRUPT_EC isr16 16
SET_INTERRUPT_EC isr17 17
SET_INTERRUPT_EC isr18 18 
SET_INTERRUPT_EC isr19 19
SET_INTERRUPT_EC isr20 20
SET_INTERRUPT_EC isr21 21
SET_INTERRUPT_EC isr22 22 
SET_INTERRUPT_EC isr23 23
SET_INTERRUPT_EC isr24 24 
SET_INTERRUPT_EC isr25 25 
SET_INTERRUPT_EC isr26 26 
SET_INTERRUPT_EC isr27 27 
SET_INTERRUPT_EC isr28 28 
SET_INTERRUPT_EC isr29 29 
SET_INTERRUPT_EC isr30 30 
SET_INTERRUPT_EC isr31 31 
SET_INTERRUPT_EC isr_timer 32 
SET_INTERRUPT_EC isr_keyboard 33 
SET_INTERRUPT_EC isr_syscall 128 

.globl __isrroutine 
__isrroutine:
	pushq %rdi
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %rbp
	pushq %rsi
	pushq %r8
	pushq %r9
	movq %rsp,%rdi
	call interrupt_handler
	popq %r9
	popq %r8
	popq %rsi
	popq %rbp
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	popq %rdi
	addq $16 ,%rsp 
	iretq
