/* © Copyright 2015, ggehlot@cs.stonybrook.edu
	 arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
	 Everyone is permitted to copy and distribute verbatim copies
	of this license document, but changing it is not allowed.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h> 
#include <syscall.h>

void exit(int arg1)
{
	syscall_0(SYS_exit);
}
