#include <stdio.h>
#include <stdlib.h>
#include <sys/dirent.h>
#include <string.h>

/*int main(int argc, char* argv[])
{	
    //printf("%s\n", argv[2]);
    char a[100] = { 0 };
    strcpy(a,argv[1]);
    //getcwd(a,100);
    dir *c = opendir(a);
    dentry *buf;
    //int e = 0;
      int flag = strcmp(argv[2],"wr_pipe");
    //printf("pipe flag is %d \n",flag);
	char *ch = argv[3];
	//char ch1[4096];
	//char *ch = ch1;
	
    while ((buf = readdir(c)) != NULL)
    {   if (flag  == 0){
                strcpy(ch,buf->name);
                ch = ch + strlen(buf->name);
		*ch = '\n';
		ch++;

	} else {
                printf("%s\n", buf->name);

	}
    }
     printf("piped data is %s",argv[3]);

    return 0;
} */

int main(int argc,char* argv[])
{
	yield();       

        char a[100] = { 0 };
	//printf("reached ls %s  %s %s \n",argv[0],argv[1],argv[2]);
	if (argc >= 2){ 
        	strcpy(a,argv[1]);
	} else {
		strcpy(a,"/rootfs/bin/");
	}

        int flag;
	if (argc == 3){
        if (strcmp(argv[2],"wr_pipe") == 0)
                flag = 1;
        else if (strcmp(argv[2],"rd_pipe") == 0)
                flag = 2;
        else
                flag = 0;
	} else {
		flag = 0;
	}
	//printf("%d",flag);

        listfiles(a,flag);
	return 0;
}
