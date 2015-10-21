#include <stdio.h>
#include <stdlib.h>
#include <sys/dirent.h>
#include <sys/string.h>

int main(int argc, char **argv){
	
        pid_t pid_1,pid_2;
	pid_1 = getpid();
	pid_2 = getppid();	
        printf("\n testing function calls %d  %d",-1009,-1);
	printf("\npid found are %d %d \n",pid_1,pid_2);

	return 1;

}
