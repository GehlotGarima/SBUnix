#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc , char* argv[])
{       
        char a[100] = { 0 };
        if (argc >= 2 ){
                strcpy(a,argv[1]);
        } else {
                strcpy(a," ");
        }

        
        int flag;
        if (strcmp(argv[2],"wr_pipe") == 0)
                flag = 1;
        else if (strcmp(argv[2],"rd_pipe") == 0)
                flag = 2;
        else
                flag = 0;

        echofiles(a,flag);
        return 0;

}


