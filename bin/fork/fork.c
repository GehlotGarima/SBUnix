#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

	int status;
	printf("In forktest file!");
	int n = fork();

        int i=0;
	for (i = 0;i<5;i++){
		int k = fork();
		printf("fork no %d,pid %d",i,k);
	        if(k) {
       	         printf("parent  ");
       	         waitpid(2, &status, 0);
        }
        else {
                printf("  child");
                //execve("bin/ps", NULL, NULL);
        }


	}	

	printf("n = %d ---- ", n); 

	int b =3;
	printf("%d", b);
	if(n) {
		printf("parent  ");
		waitpid(2, &status, 0); 
	}
	else {
		printf("  child");
		execve("bin/ps", NULL, NULL);
	}

	printf("back");

	return 0;

}
