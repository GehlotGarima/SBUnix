/* © Copyright 2015, ggehlot@cs.stonybrook.edu
	 arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
	 Everyone is permitted to copy and distribute verbatim copies
	of this license document, but changing it is not allowed.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <errno.h>

char*
getcwd(
	char *buf,
	size_t size)
{
	char *cbuf; 
	cbuf = (char *) syscall_2(SYS_getcwd,(uint64_t)buf,(uint64_t)size);
	if (cbuf == NULL)
		errno = EFAULT;

	return cbuf;
}

int
chdir(
	const char *path)
{
	int res;
	res = (int)syscall_1(SYS_chdir,(uint64_t)path);
	if (-125 <= res && res < 0){
		errno = ENOENT;
		res = -1;
	}

	return res;
}

int clear(void)
{


	int res;
        res = (int)syscall_0(SYS_clearscreen);

        return res;


}

int cls(void){

	clear();
	return 1;
}
