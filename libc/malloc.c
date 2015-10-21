/* © Copyright 2015, ggehlot@cs.stonybrook.edu
	 arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
	 Everyone is permitted to copy and distribute verbatim copies
	of this license document, but changing it is not allowed.
*/
#include <stdio.h>
#include <sys/defs.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <errno.h>

struct meta_block {
	size_t size;
	struct meta_block *next;
	struct meta_block *previous;
	int status;
	int magic_code;
};

#define HEADER_SIZE	sizeof(struct meta_block)
#define ALLOC 		1
#define FREED 		2
#define magic_alloc 	11
#define magic_freed 	22
#define magic_reall 	33

static struct meta_block *head_node=NULL;

void *cur_brk = NULL;

void *brk(int no_of_pages)
{
        void *res;
      //  void *add_in = NULL;


       res = (void *)syscall_1(SYS_brk,(uint64_t)no_of_pages);
        
     /* if (add_in) {
                cur_brk = add_in;
                res = 0;
        }
        else
                res = -1; */
	if (res == NULL)
		errno = ENOMEM;

        return res;

}

void *sbrk(ssize_t inc)
{
        void *oldbrk;
	int no_of_pages; 
	no_of_pages= (inc/4096) +1;

        /*if(cur_brk == NULL) {
                if (brk (0) < 0) {
                        return (void *)-1;
                }
        }  

        if(inc == 0)
                return cur_brk; 

         oldbrk = cur_brk;

        if(inc > 0){
		oldbrk  = brk(
                        return (void *)-1;
        } */
	oldbrk = brk(no_of_pages); 

        return oldbrk;
}


struct meta_block *block_request(struct meta_block *last,size_t size)
{
	struct meta_block *new_block;
	
	new_block = (struct meta_block *)sbrk(HEADER_SIZE + size);

	if (last) {
		last->next = new_block;
		new_block->previous=last;
	}

	new_block->status = ALLOC;
	new_block->magic_code = magic_alloc;
	new_block->size = size;
	new_block->next = NULL;

	return new_block;
}

struct meta_block *find_freed_block(struct meta_block **last,size_t size)
{
	struct meta_block *new;
	
	new = head_node;


	while (new != NULL) {

		if ( (new->status ==  FREED) && (new->size >= size) ) {
			break;
		}
		*last = new;
		new = new->next;
	}

	return new;
}

struct meta_block *block_header(void *block_pointer)
{
        struct meta_block *new;
	new = (struct meta_block *)block_pointer;
	new= new - 1;
        return new;
}


void *malloc(size_t size)
{
	struct meta_block *block;
	struct meta_block *last;
	if  (size <= 0) {
		return NULL;
	}
	
	if (head_node == NULL) {
		block = block_request(NULL,size);
		if (block == NULL) {
			return NULL;
		}
		head_node = block;
		head_node->previous = NULL;
	} else {
		last = head_node;
		block = find_freed_block(&last,size);
		if (block == NULL) {
			block = block_request(last,size); 
			if (block == NULL) {
				return NULL;
			}
		} else {  //block mannagement code
			block->magic_code = magic_reall;
			block->status = ALLOC;
		}
	}
	block++;
	return ((void *)block);
}	

void free(void *data) {
        struct meta_block *new;
        if (data == NULL) {
                return;
        }

        new = (struct meta_block *)block_header(data);
        if (new->status == FREED) {
                printf("\n double free");
                return;
        } else {
                new->status = FREED;
                if ((new->magic_code == magic_alloc) || (new->magic_code == magic_reall)) {
                         new->magic_code = magic_freed;
                } else {
                         printf("\n dangling pointer %d %d %d",new->magic_code,new->size,new->status);
                }

        }
}

