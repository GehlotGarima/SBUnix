#include <sys/sbunix.h>
#include <sys/gdt.h>
#include <sys/tarfs.h>
#include <sys/interrupts.h>
#include <sys/pmap.h>
#include <sys/vmmu.h>
#include <sys/process.h>

#define INITIAL_STACK_SIZE 4096

char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
struct tss_t tss;
extern task_struct *current;


void start(uint32_t* modulep, void* physbase, void* physfree)
{

	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;
	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
			//printf("Available Physical Memory [%x-%x]\n", smap->base, smap->base + smap->length);
			init_phy_mem((uint64_t)physbase, (uint64_t)physfree, smap->base, smap->base + smap->length);
		}
	}
//	printf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

	// kernel starts here
	clear_console();

	setup_page_tables((uint64_t)physbase);
	set_identity_paging();
	load_CR3();

	init_tarfs();
	init_process_map();
	create_init_process();


	task_struct *init_proc = create_user_process("bin/sbush");
	if(init_proc == NULL)
		while(1);

	switch_to_ring3(init_proc);

	install_timer();
        keyboard_isr();

	/* enable interrupts */ 
	__asm__ volatile("sti");

	while(1);
}

/*
#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
struct tss_t tss;
*/
void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	//register char *s, *v;
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);
	reload_gdt();
	reload_idt();
	setup_tss();
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);
	//s = "!!!!! start() returned !!!!!";
	//for(v = (char*)0xb8000; *s; ++s, v += 2) *v = *s;
	while(1);
}
