
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */


#ifndef _VMMU_H
#define _VMMU_H

#include <sys/defs.h> 

#define IDEN_V		0xFFFFFFFF00000000UL	
#define	IDEN_P		0x0UL

#define FRAME		0xFFFFFFFFFFFFF000
#define VIDEO_MEMORY    0xb8000UL
#define PAGE_SIZE	0x1000

/* Page table/directory entry flags */
#define PTE_P		0x001	// Present
#define PTE_W		0x002	// Writeable
#define PTE_U		0x004	// User
#define PTE_PWT		0x008	// Write-Through
#define PTE_PCD		0x010	// Cache-Disable
#define PTE_A		0x020	// Accessed
#define PTE_D		0x040	// Dirty
#define PTE_PS		0x080	// Page Size
#define PTE_MBZ		0x180	// Bits must be zero
#define PTE_COW		0x100	// Copy-on-write


#define PTXSHIFT	12	// offset of PTX in a linear address
#define PDXSHIFT	21	// offset of PDX in a linear address
#define PDPESHIFT	30
#define PML4SHIFT	39

#define PGT_INDEX(x, shift)	(((x) >> shift) & 0x1FF)

#define PML4_INDEX(x)	(((x) >> PML4SHIFT) & 0x1FF)
#define PDPT_INDEX(x)   (((x) >> PDPESHIFT) & 0x1FF)
#define PDT_INDEX(x)   (((x) >> PDXSHIFT) & 0x1FF)
#define PT_INDEX(x)   (((x) >> PTXSHIFT) & 0x1FF)

#define	MAP_PAGE	0
#define MAP_PROCESS	1
	
/* Page Map Level 4 Index */ 
struct PML4 {
        uint64_t pgtable_entries[512];
};

/* Page Directory Pointer Index */
struct PDPT {
        uint64_t pgtable_entries[512];
};

/* Page Direcory Index */
struct PDT {
        uint64_t pgtable_entries[512];
};

/* Page Table Index */
struct PT {
        uint64_t pgtable_entries[512];
};

/* maps the kernel */
void setup_page_tables();

/* identity map the complete physical memory */
void set_identity_paging();

/* load the page table base address */
void load_CR3();

/* allocate a physical page and map it in virtual memory */ 
void *kmalloc();

/* set/get functions cr3 */ 
void set_CR3(struct PML4 *new_cr3);
uint64_t get_CR3();

/* copies parent page table to child page table */
void setup_child_pagetable(uint64_t child_PML4);

/*set user virtual address space */
void *set_user_AddrSpace();

/* get physical address for the specified virtual address */
void* get_phy_addr(uint64_t);

/* map the elf into the virual address space */
void map_process(uint64_t, uint64_t);

/* free the virtual address and its corresponding physical page */
void kfree(uint64_t vaddr);

/* remove the page table of a process */
void delete_pagetable(uint64_t);

/* returns the physical address of the specified physical address */ 
uint64_t pa(uint64_t vaddr);

/* mask the permission bits and return the address */
uint64_t get_address(uint64_t* entry);

#endif
