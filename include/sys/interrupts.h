
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <sys/defs.h>

#define PF_P	0x1
#define PF_W	0x2
#define PF_U	0x4

struct isr_regs	{
    	uint64_t r9;
	uint64_t r8;

	uint64_t rsi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	uint64_t rdi;

	uint64_t int_no;
	uint64_t err_code;

	uint64_t rip;
    	uint64_t cs;
    	uint64_t rflags;
    	uint64_t rsp;
    	uint64_t ss;
}__attribute__((packed));

/* Defines an IDT entry */
struct idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    unsigned ist : 3;
    unsigned reserved0 : 5;
    unsigned type : 4;
    unsigned zero : 1;
    unsigned dpl : 2;
    unsigned p : 1;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved1;
} __attribute__((packed));

struct idt_ptr
{
    unsigned short limit;
    uint64_t base;
} __attribute__((packed));


extern void _x86_64_asm_ildt(struct idt_ptr* );

/* IO.C */
void outb(uint16_t port, uint16_t val);
char inb(uint16_t port);

/* ISR.C */
extern void idt_set_gate(unsigned long num, unsigned long base, unsigned short sel, unsigned char flags, unsigned int);
extern void reload_idt();
extern void init_isr();
extern void reinit_PIC();
extern void interrupt_handler(struct isr_regs *);

/* TIMER.C */
extern void install_timer();
extern void tick_timer(struct isr_regs*);

/* KEYBOARD.C */
extern void keyboard_isr();
extern void interpret_scancode(uint16_t);

#endif
