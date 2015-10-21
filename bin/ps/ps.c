#include <sys/defs.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{        	
        int flag;
	//printf("reacehd ps %s  %d",argv[1],argc);
	if (argc == 2) {
		if (strcmp(argv[1],"wr_pipe") == 0)
			flag = 1;
		else if (strcmp(argv[1],"rd_pipe") == 0)
			flag = 2;
		else 
			flag = 0;
	} else {
		flag = 0;
	}
	
	list_process(flag);
	
	return 0;

}
