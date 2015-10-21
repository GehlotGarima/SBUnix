
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */


#include <sys/sbunix.h>
#include <sys/defs.h>
#include <sys/pmap.h>
#include <sys/vmmu.h>
#include <sys/string.h> 

 
static uint64_t k_physfree;
static uint64_t max_phys;

uint64_t *mmap;

/* Set the used bit for the blocks */
void
mmap_set(uint64_t bit) {

	mmap[bit/64] |= (1ull << (bit%64));
}

/* unset the bit */
void
mmap_unset(uint64_t bit) {
	mmap[bit/64] &= ~(1ull << (bit%64));
}

/* Initialize the physical memory */
void
init_phy_mem(
	uint64_t physbase,
	uint64_t physfree,
	uint64_t base,
	uint64_t limit)
{
	k_physfree = physfree;
	max_phys = limit;
	int low_add = 0;
	int start = 0, end = 0;

	uint64_t save_kernfree = k_physfree;
        mmap = (uint64_t *) k_physfree;

        memset((void *)mmap, 0, MMAP_SIZE);
        save_kernfree += (uint64_t)(MMAP_SIZE * 64/8);
        k_physfree = save_kernfree;
	
	start = base/PAGE_SIZE;
	end = limit/PAGE_SIZE;

	mmap_unset(0);
	mmap_unset(1);
	while(start++ <= end) {

		/* if the addr is between the kernel mapping 
		 * don't use it
		 */
		if((start * PAGE_SIZE) <= k_physfree &&
			(start * PAGE_SIZE) >= physbase);
		else
			mmap_set(start);
	}

	/* unmap the lower 1Mb, these physical blocks are reserved */	
	for(start = 0; low_add < 0x100000; low_add += 0x1000) {
		start++;
		mmap_unset(start);
	}

}

static int get_first_freepage() {

	uint64_t i=0;

	for(i=0; i < MMAP_SIZE; i++) {
		uint64_t j; 
		for(j = 0; j < 64; j++) {
			uint64_t bit = 1ull << j;

			if(mmap[i] & bit)
				return i*64 + j;
		}
	}

	return -1;
}

void* allocate_page(){

	uint64_t page_addr;
	int index = get_first_freepage();

	if(index == -1)
		return NULL;

	mmap_unset(index);
	page_addr = index * PAGE_SIZE;

	return (void *)page_addr;
}

void deallocate_page(void *addr) {

	uint64_t page_addr = (uint64_t)addr;
	int index = page_addr/PAGE_SIZE;
	mmap_set(index);
} 

uint64_t get_physfree() {
	return k_physfree;
}

uint64_t get_maxphysfree() {
	return max_phys;
}
