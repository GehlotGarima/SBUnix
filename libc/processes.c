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
#include <string.h>

pid_t fork(void)
{
  pid_t return_code;
	return_code = (pid_t) syscall_0(SYS_fork);

	return return_code;	
}

pid_t
getpid(
	void)
{
	pid_t pid;
	pid = (pid_t) syscall_0(SYS_getpid);
       // uint64_t no =39;

	return pid;
}

int list_process(int flag)
{
	int ret;
	ret = (int) syscall_1(SYS_listprocess,(uint64_t)flag);
	
	return ret;
}

int listprocess(){
	list_process(0);
	return 0;
}	

pid_t
getppid(
	void)
{
	pid_t ppid;
	
	ppid = (pid_t) syscall_0(SYS_getppid);

	return ppid;
}

int
execve(
	const char *filename,
	char *const argv[],
	char *const envp[])
{      

	const char *temp = filename;
	while (temp && !((*temp == 'b') && (*(temp+1) == 'i') && (*(temp+2) == 'n'))){

		temp++;
	} 
        if (*temp == NULL || (*temp == '\0') || (*(temp+4) == '\0')) {
                printf("invalid binary path \n");
		return -1;
        }

	
	int return_code;
	return_code = (int)syscall_3(SYS_execve,(uint64_t)temp,(uint64_t)argv,(uint64_t)envp);

	if (-125 <= return_code && return_code < 0) {
                return_code = -1;
        }
	return return_code;
}
int
execvpe(
        const char *filename,
        char *const argv[],
        char *const envp[])
{        
        const char *temp = filename;
	while ( temp && !((*temp == 'b') && (*(temp+1) == 'i') && (*(temp+2) == 'n'))){
                temp++;
        }
	if (*temp == NULL || (*temp == '\0') || (*(temp+4) == '\0')) {
		printf("invalid binary path \n");
		return -1;
	}
	int return_code;

        return_code = (int)syscall_3(SYS_execve,(uint64_t)temp,(uint64_t)argv,(uint64_t)envp);

        if (-125 <= return_code && return_code < 0) {
                return_code = -1;
        }
        return return_code;
}

pid_t
waitpid(
	pid_t pid,
	int *status,
	int options)
{
	pid_t retPid;
	retPid = (pid_t)syscall_3(SYS_wait4,(uint64_t)pid,(uint64_t)status,(uint64_t)options);
/*	__asm__ volatile(
        "syscall"
        : "=a" (retPid),
          "+b" (pid),
          "+c" (status),
          "+d" (options),
	  "+e" (0)
        : "0"(SYS_wait4)
        : "cc", "memory"
        );
*/


	if (-125 <= retPid && retPid < 0) {
                retPid = -1;
        }

	return retPid;
}

unsigned int
sleep(
	unsigned int seconds)
{
	unsigned int ret;
	unsigned int ticks = seconds*1000;
	ret =(unsigned int)syscall_1(SYS_nanosleep,(uint64_t)ticks);

	return ret;
}

void kill(int pid_t)
{
	int ret;
	ret = (int)syscall_1(SYS_killprocess,(uint64_t)pid_t);
 	if (ret < 0)	
		errno = ESRCH;
}

void shutdown(){

        int ret;
        ret = (int)syscall_0(SYS_shutdown);
        if (ret < 0)
                errno = ESRCH;



}
void yield(){

        int ret;
        ret = (int)syscall_0(SYS_yield);
        if (ret < 0)
                errno = ESRCH;



}

unsigned int
alarm(
	unsigned int seconds)
{
	unsigned int retSec;
	retSec = (unsigned int)syscall_1(SYS_alarm,(uint64_t)seconds);


	return retSec;
}

