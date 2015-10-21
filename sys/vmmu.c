
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */


#include <sys/defs.h>
#include <sys/sbunix.h>
#include <sys/pmap.h>
#include <sys/vmmu.h>
#include <sys/string.h>


extern char kernmem;
extern uint64_t *mmap;

struct PML4 *pml4;
uint64_t cr3;
uint64_t top_vAddr = IDEN_V;


/* Mask the permission bits and return the address */
uint64_t get_address(uint64_t* entry)
{
    return (*entry & FRAME);
}

/* Load page table base address */
void load_CR3() {

	uint64_t base_pgdir_addr = (uint64_t)cr3;
	__asm volatile("movq %0, %%cr3":: "b"(base_pgdir_addr));	
}

/* get/set helper CR3 functions */
uint64_t get_CR3()
{
	uint64_t saved_cr3;
	__asm volatile("mov %%cr3, %0" : "=r" (saved_cr3));

	return saved_cr3;
}

void set_CR3(struct PML4 *new_cr3)
{
	uint64_t base_pgdir_addr = (uint64_t)new_cr3;
	__asm volatile("mov %0, %%cr3":: "b"(base_pgdir_addr));
}

/* allocate page directory pointer table entry */
void* pdpt_alloc(struct PML4 *pml4, uint64_t pml4_indx)
{
	struct PDPT *pdpt = (struct PDPT *)allocate_page();
	uint64_t pdpt_entry = (uint64_t)pdpt;
	pdpt_entry |= (PTE_P|PTE_W|PTE_U);
	pml4->pgtable_entries[pml4_indx] = pdpt_entry;

	return (void *)pdpt;
}

/* allocate page directory entry */  
void* pdt_alloc(struct PDPT *pdpt, uint64_t pdpt_indx)
{
	struct PDT *pdt = (struct PDT*)allocate_page();
        uint64_t pdt_entry = (uint64_t)pdt;
        pdt_entry |= (PTE_P|PTE_W|PTE_U);
        pdpt->pgtable_entries[pdpt_indx] = pdt_entry;

	return (void *)pdt;
}

/* allocate pt entry */
void* pt_alloc(struct PDT *pdt, uint64_t pdt_indx)
{
	struct PT *pt = (struct PT *)allocate_page();
        uint64_t pt_entry = (uint64_t)pt;
        pt_entry |= (PTE_P|PTE_W|PTE_U);
        pdt->pgtable_entries[pdt_indx] = pt_entry;

	return (void *)pt;
} 

void setup_page_tables(uint64_t base) {
	struct PDPT *pdpt;
	struct PDT *pdt;
	struct PT *pt;
	uint64_t vAddress = (uint64_t)&kernmem;
	uint64_t physfree = get_physfree();
	uint64_t physbase = base;

	uint64_t pml4_indx = PML4_INDEX((uint64_t)vAddress);
        uint64_t pdpt_indx = PDPT_INDEX((uint64_t)vAddress);
        uint64_t pdt_indx = PDT_INDEX((uint64_t)vAddress);

	pml4 = (struct PML4 *)allocate_page();
	cr3 = (uint64_t)pml4;
	if(!pml4)
		return;

	pdpt = pdpt_alloc(pml4, pml4_indx);
	if(!pdpt)
		return;
	
	pdt = pdt_alloc(pdpt, pdpt_indx);
	if(!pdt)
		return;

	pt = pt_alloc(pdt, pdt_indx);
	if(!pt)
		return;

	/* map the kernel from physbase to physfree */
	for(; physbase<physfree ;  physbase += 0x1000, vAddress += 0x1000) {

        	uint64_t pt_indx = PT_INDEX((uint64_t)vAddress);
		uint64_t entry = physbase;
		entry |= (PTE_P|PTE_W|PTE_U);
		pt->pgtable_entries[pt_indx] = entry;
	}

}

static void
map_virt_phys_addr(
	uint64_t vaddr,
	uint64_t paddr)
{
	struct PDPT	*pdpt;
	struct PDT	*pdt;
	struct PT	*pt;

	uint64_t pml4_indx = PML4_INDEX((uint64_t)vaddr);
	uint64_t pdpt_indx = PDPT_INDEX((uint64_t)vaddr);
	uint64_t pdt_indx = PDT_INDEX((uint64_t)vaddr);
	uint64_t pt_indx = PT_INDEX((uint64_t)vaddr);

	uint64_t pml4_entry = pml4->pgtable_entries[pml4_indx];
	if(pml4_entry & PTE_P)
		pdpt = (struct PDPT *)get_address(&pml4_entry);
	else
		pdpt = (struct PDPT*)pdpt_alloc(pml4, pml4_indx);

	uint64_t pdpt_entry = pdpt->pgtable_entries[pdpt_indx];
	if(pdpt_entry & PTE_P)
		pdt = (struct PDT*)get_address(&pdpt_entry);
	else
		pdt = (struct PDT*)pdt_alloc(pdpt, pdpt_indx);

	uint64_t pdt_entry = pdt->pgtable_entries[pdt_indx];
	if(pdt_entry & PTE_P)
                pt = (struct PT*)get_address(&pdt_entry);
        else
		pt = (struct PT*)pt_alloc(pdt, pdt_indx);

	uint64_t entry = paddr;
	entry |= (PTE_P|PTE_W|PTE_U);
	pt->pgtable_entries[pt_indx] = entry;

}

/* map the entire available memory */
void set_identity_paging() {

	uint64_t vaddr = IDEN_V;
	uint64_t paddr = IDEN_P;
	uint64_t max_phys = get_maxphysfree();

	for(; paddr <= max_phys; paddr += PAGE_SIZE, vaddr += PAGE_SIZE){
		map_virt_phys_addr(vaddr, paddr);
	}

	/* map the video memory physical address to the virtual address */
	map_virt_phys_addr((uint64_t)0xffffffff800b8000UL, VIDEO_MEMORY);

	/* map the physical address of the memory map to the same Virtual address*/ 
	mmap = (uint64_t*)(0xFFFFFFFF80000000UL | (uint64_t) mmap); 
}

/* returns the pml4 entry */
uint64_t get_pml4_entry(struct PML4 **pml4, uint64_t pml4_indx)
{
	*pml4 = (struct PML4 *)((uint64_t)*pml4 | IDEN_V);
	uint64_t pml4_entry = (*pml4)->pgtable_entries[pml4_indx];
	
	return pml4_entry;
}

/* returns the pdpt entry */
uint64_t get_pdpt_entry(struct PDPT** pdpt, uint64_t pdpt_indx)
{
	*pdpt = (struct PDPT *)((uint64_t)*pdpt | IDEN_V);
	uint64_t pdpt_entry = (*pdpt)->pgtable_entries[pdpt_indx];

	return pdpt_entry;	
}

/* returns the pdt entry */
uint64_t get_pdt_entry(struct PDT **pdt, uint64_t pdt_indx)
{
	*pdt = (struct PDT *) ((uint64_t) *pdt | IDEN_V);
	uint64_t pdt_entry = (*pdt)->pgtable_entries[pdt_indx];

	return pdt_entry;
}

/* returns the pt entry */
uint64_t get_pt_entry(struct PT **pt, uint64_t pt_indx)
{
	*pt = (struct PT *)((uint64_t) *pt | IDEN_V);
	uint64_t pt_entry = (*pt)->pgtable_entries[pt_indx];

	return pt_entry;
}

/* map the process virtual address space to the specified
 * physical address 
 */

void
map_process(
        uint64_t vaddr,
        uint64_t paddr)
{
        struct PDPT     *pdpt;
        struct PDT      *pdt;
        struct PT       *pt;

	uint64_t pml4_indx = PML4_INDEX((uint64_t)vaddr);
        uint64_t pdpt_indx = PDPT_INDEX((uint64_t)vaddr);
        uint64_t pdt_indx = PDT_INDEX((uint64_t)vaddr);
        uint64_t pt_indx = PT_INDEX((uint64_t)vaddr);

	struct PML4 *pml4 = (struct PML4*) get_CR3();

	pml4 = (struct PML4*) (IDEN_V | (uint64_t) pml4); 
        uint64_t pml4_entry = pml4->pgtable_entries[pml4_indx];

        if(pml4_entry & PTE_P)
                pdpt = (struct PDPT *)get_address(&pml4_entry);
        else
		pdpt = (struct PDPT*)pdpt_alloc(pml4, pml4_indx); 


	uint64_t pdpt_entry = get_pdpt_entry(&pdpt, pdpt_indx);
        if(pdpt_entry & PTE_P)
                pdt = (struct PDT*)get_address(&pdpt_entry);
        else
		pdt = (struct PDT*)pdt_alloc(pdpt, pdpt_indx);


	uint64_t pdt_entry = get_pdt_entry(&pdt, pdt_indx);
        if(pdt_entry & PTE_P)
                pt = (struct PT*)get_address(&pdt_entry);
        else
		pt = (struct PT*)pt_alloc(pdt, pdt_indx);


	pt = (struct PT*)((uint64_t) pt | IDEN_V);
        uint64_t entry = paddr;
        entry |= (PTE_P|PTE_W|PTE_U);

        pt->pgtable_entries[pt_indx] = entry;

}

/* allocate a virtual page to the process */ 
void* kmalloc()
{
	uint64_t page = (uint64_t)allocate_page();
	struct PML4 *newPML4 = (struct PML4 *)get_CR3();

	pml4 = newPML4;

	/* set the process CR3 to map the memory */ 
	set_CR3(pml4);

	top_vAddr += 0x1000;
	map_process(top_vAddr, page);
	memset((void *)top_vAddr, 0, 4096);


	return (void *)(top_vAddr);
}

/* set user address space, not even near to the kernel memory */
void* set_user_AddrSpace()
{
	struct PML4 *newPML4 = (struct PML4 *)allocate_page();
	struct PML4 *curPML4 = (struct PML4 *)get_CR3();

	curPML4 = (struct PML4 *)((uint64_t)curPML4 | IDEN_V);

	/* map the kernel page table for the process */
	((struct PML4 *)((uint64_t)newPML4 | IDEN_V))->pgtable_entries[511]
					= curPML4->pgtable_entries[511];

	return (void *)newPML4;
}

/*set the child page tables, copy every entry as it is */ 
void setup_child_pagetable(uint64_t child_PML4)
{
	struct PML4 *c_pml4 = (struct PML4 *)child_PML4;
	struct PML4 *p_pml4 = (struct PML4 *)get_CR3();

	int pml4_indx = 0;
	for(; pml4_indx < 510; pml4_indx++) {

		uint64_t pml4_entry = get_pml4_entry(&p_pml4, pml4_indx);

		if(pml4_entry & PTE_P) {

			struct PML4 *tmp_pml4 = (struct PML4 *)((uint64_t)c_pml4 | IDEN_V);
			struct PDPT *c_pdpt = (struct PDPT *)pdpt_alloc(tmp_pml4, pml4_indx);

			struct PDPT *p_pdpt = (struct PDPT *) get_address(&pml4_entry);
			int pdpt_indx = 0;
			for(; pdpt_indx < 512; pdpt_indx++) {

				uint64_t pdpt_entry = get_pdpt_entry(&p_pdpt, pdpt_indx);
				if(pdpt_entry & PTE_P) {

					struct PDPT *tmp_pdpt = (struct PDPT *)((uint64_t)c_pdpt | IDEN_V); 
					struct PDT *c_pdt = (struct PDT *)pdt_alloc(tmp_pdpt, pdpt_indx);

					struct PDT *p_pdt = (struct PDT *) get_address(&pdpt_entry);
					int pdt_indx = 0;
					for(; pdt_indx < 512; pdt_indx++) {

						uint64_t pdt_entry = get_pdt_entry(&p_pdt, pdt_indx);
						if(pdt_entry & PTE_P) {

							struct PDT *tmp_pdt = (struct PDT *)((uint64_t)c_pdt | IDEN_V);
							struct PT *c_pt = (struct PT *)pt_alloc(tmp_pdt, pdt_indx);
	
							struct PT *p_pt = (struct PT *)get_address(&pdt_entry);
							int pt_indx = 0;
							for(; pt_indx < 512; pt_indx++) {

								uint64_t pt_entry = get_pt_entry(&p_pt, pt_indx);
								if(pt_entry & PTE_P) {
									uint64_t page = (uint64_t)get_address(&pt_entry);
                                                                        //uint64_t entry = (uint64_t)allocate_page();

                                                                        //memcpy((void *)(entry | IDEN_V),
                                                                         //       (void *)(page | IDEN_V), 4096);
                                                                        //entry |= (PTE_P | PTE_U | PTE_COW);
									pt_entry = page | PTE_P | PTE_U | PTE_COW;	

                                    					c_pt = (struct PT *)((uint64_t)c_pt | IDEN_V);              
                                    					c_pt->pgtable_entries[pt_indx] = pt_entry;

									pt_entry = page | (PTE_P | PTE_U | PTE_COW);
									p_pt->pgtable_entries[pt_indx] = pt_entry;	
								}	
							}	
						}		
					}		 
				}	
			}	
		}
	}
	((struct PML4 *)((uint64_t)c_pml4 | IDEN_V))->pgtable_entries[511] =
                ((struct PML4 *)((uint64_t)p_pml4))->pgtable_entries[511];

        ((struct PML4 *)((uint64_t)c_pml4 | IDEN_V))->pgtable_entries[510] =
                ((struct PML4 *)((uint64_t)p_pml4))->pgtable_entries[510];
}

/* Free the memory at the specified virtual address */
void kfree(uint64_t vaddr)
{
	struct PDPT *pdpt = NULL;
	struct PDT *pdt = NULL;
	struct PT *pt = NULL;
	uint64_t paddr = 0;

	uint64_t pml4_indx = PML4_INDEX((uint64_t)vaddr);
        uint64_t pdpt_indx = PDPT_INDEX((uint64_t)vaddr);
        uint64_t pdt_indx = PDT_INDEX((uint64_t)vaddr);
        uint64_t pt_indx = PT_INDEX((uint64_t)vaddr);

	struct PML4 *pml4 = (struct PML4 *) get_CR3();
	uint64_t pml4_entry = get_pml4_entry(&pml4, pml4_indx);

	if(pml4_entry & PTE_P)
		pdpt = (struct PDPT *)get_address(&pml4_entry);

	uint64_t pdpt_entry = get_pdpt_entry(&pdpt, pdpt_indx);
	if(pdpt_entry & PTE_P)
		pdt = (struct PDT *)get_address(&pdpt_entry);

	uint64_t pdt_entry = get_pdt_entry(&pdt, pdt_indx);
	if(pdt_entry & PTE_P)
		pt = (struct PT *)get_address(&pdt_entry);

	if(pt != NULL){
		paddr = get_pt_entry(&pt, pt_indx);
		deallocate_page((void *)paddr); 
	}

	//deallocate_page((void *)pt);
	//deallocate_page((void *)pdt);
	//deallocate_page((void *)pdpt);

	top_vAddr -= 0x3000;
} 

/* map the virtual address */  
void* get_phy_addr(uint64_t vaddr)
{
	uint64_t page = (uint64_t)allocate_page();
	map_process(vaddr, page);	
	return (void *)vaddr;
}

/* delete the page tables and free the memory */
void delete_pagetable(uint64_t pml4)
{
	struct PML4 *d_pml4 =  (struct PML4 *)pml4;
        int pml4_indx = 0;
        for(; pml4_indx < 510; pml4_indx++) {

                uint64_t pml4_entry = get_pml4_entry(&d_pml4, pml4_indx);

                if(pml4_entry & PTE_P) {

                        struct PDPT *d_pdpt = (struct PDPT *) get_address(&pml4_entry);
                        int pdpt_indx = 0;
                        for(; pdpt_indx < 512; pdpt_indx++) {

                                uint64_t pdpt_entry = get_pdpt_entry(&d_pdpt, pdpt_indx);
                                if(pdpt_entry & PTE_P) {

                                        struct PDT *d_pdt = (struct PDT *) get_address(&pdpt_entry);
                                        int pdt_indx = 0;
                                        for(; pdt_indx < 512; pdt_indx++) {

                                                uint64_t pdt_entry = get_pdt_entry(&d_pdt, pdt_indx);
                                                if(pdt_entry & PTE_P) {

                                                        struct PT *d_pt = (struct PT *)get_address(&pdt_entry);
                                                        int pt_indx = 0;
                                                        for(; pt_indx < 512; pt_indx++) {

                                                                uint64_t pt_entry = get_pt_entry(&d_pt, pt_indx);
								pt_entry = (pt_entry & ~0xfff) | PTE_U | PTE_W | PTE_P;
								d_pt->pgtable_entries[pt_indx] = pt_entry; 
							}
						}
					}
				}
			}
		}
	}
}

/* returns the physical address of the 
 * corresponding Virtual address 
 */

uint64_t pa(uint64_t vaddr)
{
	uint64_t	paddr = 0;
	struct PDPT     *pdpt = NULL;
        struct PDT      *pdt = NULL;
        struct PT       *pt = NULL;

        uint64_t pml4_indx = PML4_INDEX((uint64_t)vaddr);
        uint64_t pdpt_indx = PDPT_INDEX((uint64_t)vaddr);
        uint64_t pdt_indx = PDT_INDEX((uint64_t)vaddr);
        uint64_t pt_indx = PT_INDEX((uint64_t)vaddr);

        struct PML4 *pml4 = (struct PML4*) get_CR3();
        pml4 = (struct PML4*) (IDEN_V | (uint64_t) pml4);
        uint64_t pml4_entry = pml4->pgtable_entries[pml4_indx];

        if(pml4_entry & PTE_P)
                pdpt = (struct PDPT *)get_address(&pml4_entry);

        uint64_t pdpt_entry = get_pdpt_entry(&pdpt, pdpt_indx);
        if(pdpt_entry & PTE_P)
                pdt = (struct PDT*)get_address(&pdpt_entry);


        uint64_t pdt_entry = get_pdt_entry(&pdt, pdt_indx);
        if(pdt_entry & PTE_P)
                pt = (struct PT*)get_address(&pdt_entry);

	/* page table entry is not null, extract the physical address */ 
	if(pt != NULL)
		paddr = get_pt_entry(&pt, pt_indx);

	return paddr;	
}

