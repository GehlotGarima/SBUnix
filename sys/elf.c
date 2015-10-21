#include <sys/elf.h>
#include <sys/sbunix.h>
#include <sys/tarfs.h>
#include <sys/string.h>
#include <sys/process.h>
#include <sys/vmmu.h>
#include <sys/pmap.h>
#include <sys/tarfs.h>

#define SIZE 512
 
typedef struct posix_header_ustar Header;

void* get_binary(char *filename)
{
	Header *start = (Header *)&_binary_tarfs_start;

	/* no file specified */
	if(filename == NULL)
		return NULL;

	uint64_t *end = (uint64_t *)start;
	while(*end || *(end+1) || *(end+2)){

		if(strcmp(start->name,filename) != 0) {

			int j, k=0;
			uint64_t temp = 0;

			for(j=10; j >= 0; --j)
				temp += ((start->size[j]-48) * power(8,k++));

				if(temp%512){
					temp /= SIZE;
					temp *= SIZE;
					temp += SIZE;   
				}
				start = (Header *)((uint64_t)start + 
							temp + sizeof(Header));  
				end = (uint64_t *)start; 
		}
		else
			return (void *)start;
	}

	return NULL;
}

int load_exe(task_struct *task, void *exe_start)
{
	mm_struct *mm = task->mm;

	/* ELF header */
	elfHeader *ehdr = (elfHeader *)exe_start;

	/* program header */
	progHeader *phdr = (progHeader *)((uint64_t)ehdr + ehdr->e_phoff);
	
	int count = 0;

	/* new task entry point */
	task->rip = ehdr->e_entry;
	task->mm->mmap = NULL;

	for(; count < ehdr->e_phnum; count++) {

		/* loadable section */
		if(phdr->p_type == 1) {

			vma_struct *vma = (vma_struct *)kmalloc();

			if(mm->mmap == NULL) {
				mm->mmap = vma;
			} else {
				mm->current->next = vma;
				
			}
			mm->current = vma;
			vma->mm = mm;

 			vma->start = phdr->p_vaddr;
			vma->end = phdr->p_vaddr + phdr->p_memsz;

			/* copy the exe contents to the binary's virtual address */ 
			uint64_t size = (((vma->end/0x1000)*0x1000 + 0x1000)-((vma->start/0x1000)*0x1000));
			uint64_t itr = size/0x1000;
			uint64_t start = (phdr->p_vaddr/0x1000)*0x1000;
			while(itr) {
				uint64_t page = (uint64_t)allocate_page();
				map_process(start, page);
				itr--;
				start += 0x1000; 
			}

			vma->flags = phdr->p_flags;
			vma->file = NULL;
			vma->next = NULL;
			vma->type = NOTYPE;

			if((phdr->p_flags == (PERM_R | PERM_X))
				|| (phdr->p_flags == (PERM_R | PERM_W))){

				task->mm->start_code = phdr->p_vaddr;
                                task->mm->end_code = phdr->p_vaddr + phdr->p_memsz;
				vma->file = (struct file *)kmalloc();  
				vma->file->start = (uint64_t)ehdr;
				vma->file->pgoff = phdr->p_offset;
				vma->file->size = phdr->p_filesz;	
				
				memcpy((void*)vma->start, (void*)((uint64_t)ehdr + phdr->p_offset), phdr->p_filesz);
				if(phdr->p_flags == (PERM_R | PERM_X)) {
					vma->file->bss_size = 0;
					vma->type = TEXT;
				} else {
					vma->file->bss_size = phdr->p_memsz - phdr->p_filesz;
					vma->type = DATA; 
				}	
			}

		}

		phdr++;
	}

	/* Allocate HEAP */ 
	vma_struct *vma_heap = (vma_struct *)kmalloc();
	if(mm->mmap == NULL)
		mm->mmap = vma_heap;		
	else
		mm->current->next = vma_heap;

	mm->current = vma_heap;
	vma_heap->mm = mm; 

//	if(!mm->end_data)
//		return 1;	


	vma_heap->start = HEAP_START;
	mm->brk = HEAP_START;
        vma_heap->end = HEAP_START + PAGE_SIZE;
        vma_heap->flags = (PERM_R | PERM_W);
        vma_heap->type = HEAP;
        vma_heap->file = NULL;
        vma_heap->next = NULL;
	get_phy_addr(HEAP_START);

	/* Allocate STACK */
	vma_struct *vma_stack = (vma_struct *)kmalloc();

	if(mm->mmap == NULL) {
		mm->mmap = vma_stack;
        } else {
        	mm->current->next = vma_stack;
        }
        mm->current = vma_stack;	

	uint64_t *stack = (uint64_t *)get_phy_addr(USER_STACK_TOP);	
	vma_stack->start = (uint64_t)stack + PAGE_SIZE; 
	vma_stack->end = (uint64_t)stack;
	vma_stack->flags = (PERM_R | PERM_W);
	vma_stack->type = STACK;
	vma_stack->file = NULL;
	vma_stack->next = NULL;

	task->rsp = (uint64_t)((uint64_t)stack + 4096 - 16);

	return 0;	
}

