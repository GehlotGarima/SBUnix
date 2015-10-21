
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include <sys/defs.h>
#include <sys/interrupts.h>
#include <sys/sbunix.h>
#include <sys/syscall.h>
#include <sys/string.h>
#include <sys/process.h>
#include <sys/vmmu.h>
#include <sys/elf.h>

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr_timer();
extern void isr_keyboard();
extern void isr_syscall();

extern task_struct *current;

/*
 * Declare an IDT of 256 entries.
 */

struct idt_entry idt[256];
struct idt_ptr idtp;


/*
 * Set an entry in the IDT
 */
void idt_set_gate(unsigned long num, unsigned long base, unsigned short sel,
                  unsigned char flags, unsigned int dpl)
{

	idt[num].offset_low = (base) & 0xFFFF;
        idt[num].selector = sel & 0xFFFF;
        idt[num].ist = 0;
        idt[num].reserved0 = 0;
        idt[num].type = flags;
        idt[num].zero = 0;
        idt[num].dpl = dpl;
        idt[num].p = 1;
        idt[num].offset_mid = (base >> 16) & 0xFFFF;
        idt[num].offset_high = (base >> 32) & 0xFFFFFFFF;


}

/* Installs the IDT */
void reload_idt()
{
	/* Set up the IDT pointer */
	idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
	idtp.base = (uint64_t)&idt;

	/* Initialize IDT table entries to zero */
	memset(&idt, 0, sizeof(struct idt_entry) * 256);

 	init_isr();

    __asm__ __volatile__ (  "lidt (%0)\n"
                            :
                            :"r"(&idtp)
                            :
                            );
	/* load the idt table */
	//_x86_64_asm_ildt(&idtp);

}


void reinit_PIC()
{
	
	/* ICW1 - starts the initialization sequence */
	outb(0x20, 0x11);
	outb(0xA0, 0x11);

	/* ICW2 - remap offset address of IDT */
	outb(0x21, 0x20); // Master PIC vector offset
	outb(0xA1, 0x28); // Slave PIC vector offset

	/* ICW3 - Setup cascading */	
	outb(0x21, 0x04); // informs Master PIC about Slave PIC cascaded at ICW2 
	outb(0xA1, 0x02); // cascade identity of Slave PIC

	/* ICW4 - environment info */		
	outb(0x21, 0x01);
	outb(0xA1, 0x01);

	/* mask interrupts */
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

}

void init_isr()
{
	idt_set_gate(0, ((uint64_t)isr0),0x08, 0xe, 0);
	idt_set_gate(1, ((uint64_t)isr1),0x08, 0xe, 0);
	idt_set_gate(2, ((uint64_t)isr2),0x08, 0xe, 0);
	idt_set_gate(3, ((uint64_t)isr3),0x08, 0xe, 0);
	idt_set_gate(4, ((uint64_t)isr4),0x08, 0xe, 0);
	idt_set_gate(5, ((uint64_t)isr5),0x08, 0xe, 0);
	idt_set_gate(6, ((uint64_t)isr6),0x08, 0xe, 0);
	idt_set_gate(7, ((uint64_t)isr7),0x08, 0xe, 0);
	idt_set_gate(8, ((uint64_t)isr8),0x08, 0xe, 0);
	idt_set_gate(9, ((uint64_t)isr9),0x08, 0xe, 0);
	idt_set_gate(10, ((uint64_t)isr10),0x08, 0xe, 0);
	idt_set_gate(11, ((uint64_t)isr11),0x08, 0xe, 0);
	idt_set_gate(12, ((uint64_t)isr12),0x08, 0xe, 0);
	idt_set_gate(13, ((uint64_t)isr13),0x08, 0xe, 0);
	idt_set_gate(14, ((uint64_t)isr14),0x08, 0xe, 0);
	idt_set_gate(15, ((uint64_t)isr15),0x08, 0xe, 0);
	idt_set_gate(16, ((uint64_t)isr16),0x08, 0xe, 0);
	idt_set_gate(17, ((uint64_t)isr17),0x08, 0xe, 0);
	idt_set_gate(18, ((uint64_t)isr18),0x08, 0xe, 0);
	idt_set_gate(19, ((uint64_t)isr19),0x08, 0xe, 0);
	idt_set_gate(20, ((uint64_t)isr20),0x08, 0xe, 0);
	idt_set_gate(21, ((uint64_t)isr21),0x08, 0xe, 0);
	idt_set_gate(22, ((uint64_t)isr22),0x08, 0xe, 0);
	idt_set_gate(23, ((uint64_t)isr23),0x08, 0xe, 0);
	idt_set_gate(24, ((uint64_t)isr24),0x08, 0xe, 0);
	idt_set_gate(25, ((uint64_t)isr25),0x08, 0xe, 0);
	idt_set_gate(26, ((uint64_t)isr26),0x08, 0xe, 0);	
	idt_set_gate(27, ((uint64_t)isr27),0x08, 0xe, 0);
	idt_set_gate(28, ((uint64_t)isr28),0x08, 0xe, 0);
	idt_set_gate(29, ((uint64_t)isr29),0x08, 0xe, 0);
	idt_set_gate(30, ((uint64_t)isr30),0x08, 0xe, 0);
	idt_set_gate(31, ((uint64_t)isr31),0x08, 0xe, 0);

	idt_set_gate(32, ((uint64_t)isr_timer),0x08, 0xe, 0);
        idt_set_gate(33, ((uint64_t)isr_keyboard),0x08, 0xe, 0);

	idt_set_gate(128, ((uint64_t)isr_syscall),0x08, 0xe, 3);
	
	reinit_PIC();
}

void pagefault_handler(struct isr_regs *reg)
{
	volatile uint64_t faultAddr;
	int allocate_heap = 0;
	int grow_stack = 0;

	__asm volatile("mov %%cr2, %0" : "=r" (faultAddr));


	uint64_t err = reg->err_code & 0xF;

	vma_struct *vma = current->mm->mmap;

	// COW page fault handling
	if((err & PF_P) && (err & PF_W)) {

		uint64_t pte_entry = pa(faultAddr);
		if(pte_entry & PTE_COW) {
			uint64_t temp_vaddr = (uint64_t)kmalloc();
			uint64_t paddr = pa(get_address(&temp_vaddr));
			//uint64_t v = (faultAddr&(0xffffffffffffffffUL<<12));
			memcpy((void *)temp_vaddr, (void *)(faultAddr&(0xffffffffffffffffUL<<12)), 4096);
			map_process(faultAddr&(0xffffffffffffffffUL<<12), paddr);

			pte_entry = (pte_entry & ~0xfff)| PTE_P | PTE_W | PTE_U; 
			return;
		}	
	}

      	while(vma) {

		/* Dynamic Heap Allocation */
                if(vma->type == HEAP && faultAddr >= vma->start &&
                                faultAddr <= HEAP_END) {
                        uint64_t *heap = (uint64_t *)get_phy_addr(vma->end);
                        vma->end = ((uint64_t)heap) + 0x1000;
			allocate_heap = 1;
                        break;
                }

		/* Auto grow stack */
                if(vma->type == STACK && faultAddr <= vma->end &&
                        (vma->start - vma->end - PAGE_SIZE) <= STACK_LIMIT){

                        uint64_t *stack = (uint64_t *)get_phy_addr(vma->end - PAGE_SIZE);
                        vma->end = (uint64_t)stack;
			grow_stack = 1;
                        break;
                }

		vma = vma->next;
	}

	

	if(!(err & PF_P) && !(allocate_heap || grow_stack)) {
		printf("\nSegmentation fault\n");
		exit(1);	
	}
	
}

void gpf(struct isr_regs *reg)
{
	printf("General Protection Fault\n");

	uint64_t faultAddr;
	uint64_t cr3;

	__asm volatile("mov %%cr2, %0" : "=r" (faultAddr));
	__asm volatile("mov %%cr3, %0" : "=r" (cr3));

	printf("\n gpf cr3 %p\n", cr3);
	printf("\n gpf cr2 %p\n", faultAddr);

	while(1);
}

void intr_6(struct isr_regs *reg)
{

        printf("Illegal Opcode\n");
}

static void
execute_interrupt(struct isr_regs *reg)
{
	/*
         * handle the timer interrupt
	 */
	if (reg->int_no == 32){
		tick_timer(reg);
	}


	/*
	 * handle the keyboard interrupt
	 */

	if(reg->int_no == 33) {
		keyboard_isr();
	}

	if(reg->int_no == 128) {
		syscall_handler(reg);
	}

	
	/* For trap Numbers above 39, 
	 * send EOI to slave interrupt controller
	 */

	if (reg->int_no >= 40)
    	{
        	outb(0xA0, 0x20);
   	}

	if(reg->int_no == 14) {
                pagefault_handler(reg);
        }

	if(reg->int_no == 13) {
		gpf(reg);
	}	

	/*
	 * send EOI to the master interrupt controller
	 */
	outb(0x20, 0x20);
}

void
interrupt_handler(struct isr_regs *reg)
{
	/* TO-DO: will be handling panic, page faults and
	 * system calls before dispatching the trap
	 */

	/* common interrupt handler for all traps*/
	execute_interrupt(reg);
}
