
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */


#ifndef _ELF_H
#define _ELF_H

#include <sys/defs.h>
#include <sys/process.h>

enum vma_types {

	TEXT,
	DATA,
	STACK,
	HEAP,
	NOTYPE
};

//Segment Flags
#define PERM_X  0x1 //Execute
#define PERM_W  0x2 //Write
#define PERM_R  0x4 //Read

typedef struct elf64_Header
{
	unsigned char 	e_ident[16];		/* ELF identification */
	uint16_t	e_type;			/* Object file type */
	uint16_t	e_machine;		/* Machine type */
	uint32_t	e_version;		/* Object file version */
	uint64_t	e_entry;		/* Entry point virtual address */
	uint64_t	e_phoff;		/* Program header table offset */
	uint64_t	e_shoff;		/* Section header table offset */
	uint32_t	e_flags;		/* Processor-specific flags */
	uint16_t	e_ehsize;		/* ELF header size in bytes*/
	uint16_t	e_phentsize;		/* Size of program header entry */
	uint16_t	e_phnum;		/* Number of program header entries */
	uint16_t	e_shentsize;		/* Size of section header entry */
	uint16_t	e_shnum;		/* Number of section header entries */
	uint16_t	e_shstrndx;		/* Section name string table index */
}elfHeader;

typedef struct prg64_Header {

	uint32_t	p_type;			/* Type of segment */	
	uint32_t	p_flags;			/* Segment attributes */
	uint32_t	p_offset;			/* Offset in file */
	uint64_t	p_vaddr;			/* Virtual address in memory */	
	uint64_t	p_paddr;
	uint64_t	p_filesz;		/* Size of segment in file */	
	uint64_t	p_memsz;		/* Size of segment in memory */
	 uint64_t	align;
}progHeader; 


void* get_binary(char *exe_name);

int load_exe(task_struct *, void *);

#endif
