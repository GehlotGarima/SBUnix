#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
	
        char a[100] = { 0 };
	if (argc >= 2) {
        	strcpy(a,argv[1]);
	} else {
		strcpy(a,"/rootfs/etc/help");
	}
	

        int flag;
	if (argc >= 3){
        if (strcmp(argv[2],"wr_pipe") == 0)
                flag = 1;
        else if (strcmp(argv[2],"rd_pipe") == 0)
                flag = 2;
        else
                flag = 0;
	} else {
		flag = 0;
	}
	//printf("%s  \n",argv[2]);
		

        catfiles(a,flag);
        return 0;

}


