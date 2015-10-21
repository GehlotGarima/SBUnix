
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */


#ifndef _PMAP_H
#define _PMAP_H


#define MMAP_SIZE	256

/* set the physical memory limits */
void init_phy_mem(uint64_t physbase, uint64_t physfree, uint64_t base, uint64_t limit);

/* map/unmap the meory region */
void mmap_set(uint64_t bit);
void mmap_unset(uint64_t bit);

/* allocate/deallocate physical page */
void* allocate_page();
void deallocate_page(void *addr);

/* helper functions for memory mapping */ 
uint64_t get_physfree();
uint64_t get_maxphysfree();


#endif
