#ifndef _PROCESS_H
#define _PROCESS_H

#include <sys/defs.h>
#include <sys/interrupts.h>
#include <sys/dirent.h>

#define MAX_FD			100
#define MAX_PROCESS		100

#define PAGE_SIZE		0x1000

#define USER_STACK_TOP		0xF0000000
#define HEAP_START		0x08000000

#define STACK_LIMIT             8192
#define HEAP_END		0x10000000

/* Task State */
#define TASK_NEW                1
#define TASK_READY		1
#define TASK_RUNNING		2
#define TASK_INTERRUPTIBLE	3
#define TASK_UNINTERRUPTIBLE	4
#define TASK_STOPPED		5
#define TASK_ZOMBIE		6
#define TASK_WAITING		7

int processID[MAX_PROCESS];

typedef struct task_struct task_struct; 
typedef struct vm_area_struct vma_struct;
typedef struct mm_struct mm_struct;

struct file{
    uint64_t   start;       /* start address of region */
    uint64_t   pgoff;       /* offset in file or NULL */
    uint64_t   size;        /* region initialised to here */
    uint64_t   bss_size;
};

struct mm_struct {
	vma_struct *mmap, *current;
	uint64_t start_code, end_code, start_data, end_data;
	uint64_t start_brk, brk, start_stack;
	uint64_t arg_start, arg_end, env_start, env_end;
	uint64_t rss, total_vm, locked_vm;
};

struct vm_area_struct {
	mm_struct *mm;
	uint64_t start;              // Our start address within vm_mm
	uint64_t end;                // The first byte after our end address within vm_mm
	vma_struct *next;            // linked list of VM areas per task, sorted by address
	uint64_t flags;              // Flags read, write, execute permissions
	uint64_t type;               // type of segment its refering to
	struct file *file;           // reference to file descriptors for file opened for writing
};

struct task_struct {

	int pid;
	int ppid;
	uint64_t kernel_stack;
	uint64_t init_kern;
	uint64_t rip;			// instruction pointer
	uint64_t rsp;			// process stack pointer
	uint64_t task_state;            // Saves the current state of task
	uint64_t sleep;			// sleep time
	mm_struct* mm;			// pointer to mm_struct 
	uint64_t cr3;
	char pname[15];                 // Name of task
	task_struct* next;              // The next process in the process list
	struct fd* fd[MAX_FD];		// array of file descriptor pointers
	int wait_on_child_pid; 	        // pid of child last exited
	uint32_t nchild;                // Number of children
        uint64_t alarm;                 // initialize alarm
        char cur_dir[30];
        file_t *cur_node;

};

int getProcessID();

void init_process_map();

void create_init_process();

task_struct* create_user_process(char *filename);

void switch_to_ring3(task_struct*);

void schedule();

int  sys_fork();

int  sys_execve(char *, char **, char **);

void decrement_sleep_time();

void nano_sleep(uint64_t sleep_time);

void exit(int);

void init_func();

void kill_process();

void exit_task(task_struct *task);

#endif
