
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */


#include <sys/vmmu.h>
#include <sys/process.h>
#include <sys/gdt.h>
#include <sys/string.h>
#include <sys/sbunix.h>
#include <sys/elf.h>
#include <sys/tarfs.h>
#include <sys/pmap.h>
#include <sys/syscall.h>
#include <sys/elf.h>

task_struct *zombie_task;
task_struct *sleep_task = NULL;
task_struct *current;
task_struct *task_remove;

task_struct *first;

/* get the available process ID */ 
int getProcessID()
{
	int id = 0;
	for(;id < MAX_PROCESS; id++) {
		if(processID[id] == 0) {
			processID[id] = 1;
			return id;
		}
	}
	return -1;
}

/* mark all the 100 entries as unused */
void init_process_map()
{
	int id = 0;
	for(; id < MAX_PROCESS; id++)
		processID[id] = 0;
}

void flush_zombies()
{

	if(zombie_task == NULL)
		return;

	task_struct *task;
	while(zombie_task) {
		task = zombie_task;		
		zombie_task = zombie_task->next;	
		exit_task(task);	
	}	
}

/* init function */
void init_func(){
	while(1) {
		schedule();
	}

}

/* create the first kernel process */
void create_init_process()
{
	/* init process task struct */
	task_struct *init = (task_struct *) kmalloc();

	init->task_state = TASK_RUNNING;
	init->pid = getProcessID();
	init->ppid = 0;
	init->next = init;
	init->mm = NULL;
	init->sleep = 0;
	init->alarm = 0;
	init->wait_on_child_pid = -1;
	init->cr3 = (uint64_t)get_CR3();
	init->rip = (uint64_t)&init_func;
	init->nchild = 0;
	strcpy(init->pname,"init");

	first = init;

	/* set the kernel stack */
	void *kstack = (void *)kmalloc();
	init->rsp = init->init_kern = init->kernel_stack = ((uint64_t)kstack +0x1000 - 16);

	current = init;
}

/* create the user process */
task_struct* create_user_process(char *filename)
{
	/* read the elf header from the provided binary */ 
	struct posix_header_ustar *file = get_binary(filename);

	if(file == NULL) {
		printf("Binary not found!\n");
		return NULL;
	}

	int pid = getProcessID();

	/* new user process */
	task_struct *newProc = (task_struct *)kmalloc();
	newProc->pid = pid;
	newProc->task_state = TASK_READY;
	newProc->wait_on_child_pid = -1;
	newProc->nchild = 0;
	newProc->alarm = 0;

	/* set current directory of the process */
        strcpy(newProc->cur_dir,"/rootfs/bin/");
        char temp[30] = "\0";
        strcpy(temp,newProc->cur_dir);

	/* set the root node */
        dir *ret_dir = sys_opendir(temp);
        newProc->cur_node = ret_dir->node;
        strcpy(newProc->pname, filename);

	/* set the user address space */	
	newProc->cr3 = (uint64_t)set_user_AddrSpace();
	struct PML4 *curr_CR3 = (struct PML4 *)get_CR3();

	/* set the current CR3 value to new process pml4 address */
	set_CR3((struct PML4 *)newProc->cr3);

	/* allocate space to mm structure */ 
	mm_struct *mm = (mm_struct *)kmalloc();
	newProc->mm = mm;

	/* kernel stack, set rsp to the base address of the stack */ 
	void *kStack = (void *)kmalloc();
	newProc->init_kern = newProc->kernel_stack = ((uint64_t)kStack + 4096 - 16);

	/* load the elf */
	int error = load_exe(newProc, (void *)(file + 1));

	/* return if some error occured while loading the exe */
	if(error)
		return NULL;

	/* set the CR3 value back to the prev process pml4 */ 
	set_CR3((struct PML4 *)curr_CR3);

	/* make the new process as the current process */
	current->next = newProc;
	newProc->next = current;
	current = newProc; 

	return newProc;
}

/* Switch to Ring 3 */ 
void switch_to_ring3(task_struct *task)
{
	task->task_state = TASK_RUNNING;

        volatile unsigned int a = 0x2B;

        __asm volatile("movq %0,%%rax;\n\t"
            "ltr (%%rax);"::"r"(&a));


	/* set the kernel stack for the process */ 
	tss.rsp0 = task->init_kern;
	

	/* set the ds, es, fs and gs registers 
	 * with the 0x23 Data segment value for
	 * user space and 0x1b code segment value
	 */
	__asm__ __volatile__ (
	"sti;"
        "movq %0, %%cr3;"
        "mov $0x23, %%ax;"
        "mov %%ax, %%ds;" /* data segment values */
        "mov %%ax, %%es;"
        "mov %%ax, %%fs;"
        "mov %%ax, %%gs;"

        "movq %1, %%rax;"
        "pushq $0x23;"
        "pushq %%rax;"
        "pushfq;"
        "popq %%rax;"
        "orq $0x200, %%rax;"
        "pushq %%rax;"
        "pushq $0x1B;"
        "pushq %2;"
	"movq $0x0, %%rdi;"
	"movq $0x0, %%rsi;" 
        "iretq;"
        ::"r"(task->cr3), "r"(task->rsp),"r"(task->rip)
    );

}

/* copy the task structure of parent to the child */

void* copy_task_struct(task_struct *parent)
{
	task_struct *child_task = (task_struct *)kmalloc();

	child_task->task_state = TASK_READY;
	child_task->pid = getProcessID();
	child_task->ppid = parent->pid ;
	child_task->next = NULL;
	child_task->mm = NULL;
	child_task->sleep = 0;
	child_task->alarm = 0;
        child_task->wait_on_child_pid = -1;
	parent->nchild += 1;
	child_task->nchild = 0;
	strcpy(child_task->pname,current->pname);

	/* set child page tables */
	child_task->cr3 = (uint64_t)allocate_page();
	setup_child_pagetable(child_task->cr3);

	/* set the child CR3 to allocate memory to its structures*/
	set_CR3((struct PML4 *)child_task->cr3);

	/* copy file descriptors */
	memcpy(&(child_task->fd[0]),&(parent->fd[0]),(sizeof(parent->fd[0])* MAX_FD));

	/* copy mm_struct */ 
	child_task->mm = kmalloc();
	memcpy(child_task->mm, parent->mm, sizeof(mm_struct));

	/* copy vmas */
	vma_struct *parent_vma = parent->mm->mmap;
	vma_struct *child_vma = NULL;
	int first = 0;

	while(parent_vma) {

		if(child_vma == NULL)
			first = 1;

		child_vma = (vma_struct *)kmalloc();

		memcpy(child_vma, parent_vma, sizeof(vma_struct));

		if(parent_vma->file!=NULL){
			child_vma->file = kmalloc();
			memcpy(child_vma->file,parent_vma->file,sizeof(struct file));
		}

		if(first) {
			child_task->mm->mmap = child_vma;
			first = 0;
		}

		if(child_vma->next)
			child_vma = child_vma->next;
		parent_vma = parent_vma->next;
	}

	/* if no vma was there in parent , mark the
	 * child memory map as null and return
	 */
	if(!child_vma) {
		child_task->mm->mmap = NULL;
		goto ret;
	}		

	child_vma->next = NULL;
	
ret:
	return (void *)child_task;

}

int sys_fork()
{
	task_struct *temp, *parent = NULL;	

	/* copy parent's task_struct to Child task structure */
	task_struct *child = (task_struct *)copy_task_struct((task_struct*)current);

	/* queue the child task just after the parent(current) task */
	temp = current->next;
	current->next = child;
	child->next = temp;

	parent = current;

	/* allocate the meory for child kernel stack and 
	 * copy the parent stack content to it
	 */

	void *kstack = (void *)kmalloc();
        child->init_kern = child->kernel_stack = ((uint64_t)kstack + 4096 - 16);
        memcpy((void *)(child->init_kern - 0x1000 +16), (void *)(parent->init_kern - 0x1000 +16), 4096-16);

	volatile uint64_t stack_pos;
	/* load back the parent CR3 */
	set_CR3((struct PML4 *)parent->cr3);

	/* Below assembly copy the current rsp
	 * to the variable to adjust the child's
	 * kernel stack position and the and load
	 * the next instruction pointer in the child's
	 * rip, so that when child  is scheduled to 
	 * execute, it starts from the next instruction
	 */

	__asm__ __volatile__(
                "movq $2f, %0;"
		"2:\t"
                :"=g"(child->rip)
        );

	__asm__ __volatile__(
                "movq %%rsp, %0;"
                :"=r"(stack_pos)
        );

	
     
	if(current == parent) {
		/* if parent is executing, set the child kernel stack
		 * before leaving the ring 0
		 */
		child->kernel_stack = child->init_kern - (parent->init_kern - stack_pos);
       		return child->pid;
	}
	else {
       		outb(0x20, 0x20);
        	return 0;
  	}
}

void context_switch(task_struct *next, task_struct *prev) {


	/* set the kernel stack */ 
	tss.rsp0 = next->init_kern;

	/* save the rsp pointer of the swapped process */
	__asm__ __volatile__ (
		"sti;"	
		"movq %%rsp, (%0)"
		::"r"(&(prev->kernel_stack))
	);

	/* move the stack pointer of current process to rsp register */ 
	__asm__ __volatile__ (
		"movq %0, %%rsp;"
		::"r"(next->kernel_stack)
	
	);

	/* move the current process page table base address to cr3 register */
	__asm__ __volatile__ (
		"movq %0, %%cr3;"
		::"r"(next->cr3)
	);

	/*
	 * save the instruction pointer for swapped process 
	 * and push the current process instruction pointer
	 * on the kernel stack
	 */
	__asm__ __volatile__ (
		"movq $1f, %0;"
		"pushq %1;"
		"retq;"
		"1:\t"
		:"=g"(prev->rip)
		:"r"(next->rip)
	);	

}



void schedule()
{

	int indx = 0;
	if(indx%50 == 0)
		flush_zombies();

	indx++;

	task_struct *prev =  (task_struct*)current;

	current = current->next;
	if(current != prev)
		context_switch( (struct task_struct*)current, prev);

}


void free_memory_map(mm_struct *mm)
{
	if(mm->mmap == NULL)
		return;

	vma_struct *vma = mm->mmap;
	while(vma != NULL) {
		vma_struct *free_vma = vma;
		vma = vma->next;
		kfree((uint64_t)free_vma);
	}	
	
}


int sys_execve(char *filename, char **argv, char **envp)
{
	int i=0;
	uint64_t argc=0;

	char c_ptr[6][64];
	
	for (i = 0; i < 6; ++i)
       		c_ptr[i][0] = '\0';


	/* push the filename on the userstack */ 
	strcpy(c_ptr[argc], filename);
	uint64_t c_argc = 0;
	argc++;

	/* if arguments passed are not NULL copy them */ 
	if(argv != NULL) {
		while(argv[c_argc] != NULL){
              		strcpy(c_ptr[argc],argv[c_argc]);
               		argc++;
			c_argc++;
        	}
	}


    	  
	/* create a new task structure */
	task_struct *task = (task_struct *)kmalloc();

	/* copy pid and ppid */
	task->pid = current->pid;
	task->ppid = current->ppid;
	strcpy(task->pname,filename);

	/* copy file descriptors */ 
	memcpy(&(task->fd[0]),&(current->fd[0]),(sizeof(current->fd[0])* MAX_FD));

        /* read the elf header from the provided binary */
        struct posix_header_ustar *file = get_binary(filename);

        if(file == NULL) {
                printf("Binary not found!\n");
                return -1;
        }

	task->cr3 = (uint64_t)set_user_AddrSpace();
        struct PML4 *curr_CR3 = (struct PML4 *)get_CR3();

	set_CR3((struct PML4 *)task->cr3);

        /* allocate a new page to the kernel stack */
        void *kstack = (void *)kmalloc();
        task->init_kern = task->kernel_stack = ((uint64_t)kstack + 4096 - 16);

        /* allocate space to mm structure */
        mm_struct *mm = (mm_struct *)kmalloc();
        task->mm = mm;

        /* load the elf */
        int error = load_exe(task, (void *)(file + 1));

        /* return if exe is not loaded properly */
        if(error)
                return -1;

	void *ptr = (void *)(USER_STACK_TOP + 0x1000 - 16 - sizeof(c_ptr));
        memcpy(ptr, (void *)c_ptr, sizeof(c_ptr));

	i=argc;
        for(; i>0; i--){

		*(uint64_t*)(ptr - 8*i) = (uint64_t)ptr + (argc-i)*64;
        }

       	ptr = ptr- 8*argc;
        task->rsp = (uint64_t)ptr;

	set_CR3((struct PML4 *)curr_CR3);

	/* free memory allocated to vmas */
        //free_memory_map(current->mm);
	//current->mm->mmap = NULL;

        /* All the vmas are freed, now free the memory map */
        //kfree((uint64_t)current->mm);
        //current->mm = NULL;

        /* free the page  tables */
        //delete_pagetable(current->cr3);

	task_struct *prev = current->next;
	while(prev->next != current)
		prev = prev->next;
	

	prev->next = task;
	task->next = current->next;

	current = task;

	/* set current directory of the process */
        strcpy(task->cur_dir,"/rootfs/bin");
        char temp[30] = "\0";
        strcpy(temp,task->cur_dir);

	/* set wait_on_child_pid to -1 */
	task->wait_on_child_pid = -1;
	task->task_state = TASK_RUNNING;


        /* set the task state segment register */
        tss.rsp0 = task->init_kern;

	 __asm__ __volatile__ (
	"sti;"
	"movq %0, %%cr3;"
        "movq %1, %%rsp;"
        "mov $0x23, %%ax;"
        "mov %%ax, %%ds;"
        "mov %%ax, %%es;"
        "mov %%ax, %%fs;"
        "mov %%ax, %%gs;"

        "movq %2, %%rax;"
        "pushq $0x23;"
        "pushq %%rax;"
        "pushfq;"
        "popq %%rax;"
        "orq $0x200, %%rax;"
        "pushq %%rax;"
        "pushq $0x1B;"
        "pushq %3;"
        "movq %4, %%rsi;"
        "movq %5, %%rdi;"
        "iretq;"
        ::"r"(task->cr3), "r"(task->kernel_stack), "r"(task->rsp), "r"(task->rip),"r"(ptr), "r"(argc)
    );

    return -1;

}

void nano_sleep(uint64_t stime)
{
	current->sleep = stime;
	task_struct *temp = current;

	/* remove it from the ready list */
	while(temp->next !=  current) {
		temp = temp->next;	
	}

	temp->next = current->next;

	/* add it to the sleeping list */
	current->next = sleep_task;

	__asm__ __volatile__("movq %1, %%rax;"
                                         "movq %%rax, %0;"
                                        :"=r"(sleep_task)
                                        :"r"(current)
                                        :"%rax"
                        );

	sleep_task = current;
	sleep_task->task_state = TASK_WAITING;

	/* mark the next task as the current */
	current = temp->next;

	printf("");

	context_switch(current, sleep_task);

} 
	 
void decrement_sleep_time()
{
	task_struct *temp, *task;
	temp = sleep_task;
	task = current;

	while(temp) {

			if(temp->sleep <= 0 && temp->nchild == 0) {

			__asm__ __volatile__("movq %1, %%rax;" 
     					 "movq %%rax, %0;"
     					:"=r"(sleep_task)
     					:"r"(temp->next)
    					:"%rax" 
     			);
		
                       	temp->next = task->next;
                       	task->next = temp;

			current = task->next;
			context_switch(current, task);
			break;	

			} else {	
				uint64_t s_time = temp->sleep;
				s_time--;
				temp->sleep = s_time;
				if ( temp->alarm == 1 && temp->sleep == 0) {
						temp->alarm = 0;
                                                printf("\n \n The ALARM is going off now \n");
                                }
	
				temp = temp->next;
			}

	}

	return;

}


/* free the task memory */

void exit_task(task_struct *task)
{

	free_memory_map(task->mm);
	kfree((uint64_t)task->mm);
	task->mm = NULL;
	
//	delete_pagetable(task->cr3);

	kfree((uint64_t)task->init_kern);

	kfree((uint64_t)task);

}

/*
 * check if any process is waiting on your pid 
 * If found, move that process to the ready list to
 * resume execution
 */
 
void cleanup()
{
//	task_struct *task_remove = current;
	task_struct *ptr = task_remove->next;
	task_struct *parent = NULL, *temp;

	temp = sleep_task;

	while(ptr->next != task_remove)
		ptr = ptr->next;	

	/*
	 * check if the sleep list is empty,
	 * if empty, no need to traverse 
	 */ 
	if(sleep_task) {

		parent = temp->next;
		if(temp->wait_on_child_pid == task_remove->pid) {
			//sleep_task = sleep_task->next;
			__asm__ __volatile__("movq %1, %%rax;"
                                         "movq %%rax, %0;"
                                        :"=r"(sleep_task)
                                        :"r"(temp->next)
                                        :"%rax"
                        );
			temp->nchild -= 1;
			parent = temp;
		} else { 

			while(parent) {
			
       	        	 	if(parent->wait_on_child_pid == task_remove->pid)
       		         	{
				
					parent->nchild -= 1;
					parent->task_state = TASK_RUNNING;
					parent->wait_on_child_pid = -1;
					temp->next = parent->next;
				}

               			parent = parent->next;
				temp = temp->next;
        		}
		}
	}

	

	if(parent) {
		parent->next = task_remove->next;
		ptr->next = parent;
		delete_pagetable(parent->cr3);	
	}
	else
		ptr->next = task_remove->next;


	if(task_remove->nchild > 0) {

		task_struct *child = ptr->next;

		while(child->next != ptr->next) {

			if(child->ppid == task_remove->pid)
				child->ppid = 0;

			child = child->next;	
		}

		child = sleep_task;

		while(child) {
			if(child->ppid == task_remove->pid) 
                                child->ppid = 0;
		
			child = child->next;
		}	
	}

	if(task_remove->ppid == 0) {
		task_remove->task_state = TASK_ZOMBIE;
		if(zombie_task == NULL){
			zombie_task = task_remove;
			zombie_task->next = NULL;
		} else {
	
			task_struct *task = zombie_task;
			while(task->next) 
				task = task->next;

			task->next = task_remove;
		}

	}

	/* set the current process */
        current = ptr->next; 
	
	/* free the process memory */
	if(task_remove->nchild == 0 && task_remove->ppid != 0) 
		exit_task(task_remove);

	processID[task_remove->pid] = 0;

	/* schedule the process */
	context_switch(current, task_remove);	
}

void kill_process(int pid,int flag)
{
	int running = 0;
	int sleep = 0;

	if(current->pid == pid) {
		running = 1;
		task_remove = current;
		goto cleanup;
	}

	task_struct *prev = current;

	while(prev->next != current) {
		if(pid == prev->pid) {

			task_remove = prev;
			running = 1;	
			break;
		}
		prev = prev->next;
	}

	if(prev->pid == pid) {
		task_remove = prev;
		running = 1;
	}
		

cleanup:
	if(running) {
		if (flag)
			printf("[1]+  Killed: 9\n");		
		cleanup();
	}


	/* search in the sleep list */

	if(sleep_task->pid == pid) {
		sleep = 1;
		task_remove = sleep_task;
		sleep_task = sleep_task->next;
		goto out;
	}

	task_struct *temp = sleep_task;
	while(temp->next) {

		if(temp->next->pid == pid) {
			sleep = 1;
			task_remove = temp->next;
			temp->next = temp->next->next;	
			break;	
		}
		temp = temp->next;
	}

out:
	if(sleep) {
		printf("[1]+  Killed: 9\n");
		processID[task_remove->pid] = 0;	
		exit_task(task_remove);
	}
	else 
		printf("****ERROR: No such process found!****\n");	
}

void exit(int exit_status)
{

	task_remove = current;

	cleanup();
}
