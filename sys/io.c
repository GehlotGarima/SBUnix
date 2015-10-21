
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include <sys/interrupts.h>

void
outb(uint16_t port, uint16_t val)
{
	__asm__ volatile ("outb %%al, %%dx"::"d" (port), "a" (val));

}

char
inb(uint16_t port) {
	char ret;

	__asm__ volatile ("inb %%dx,%%al":"=a"(ret):"d"(port));

	return ret;
}
