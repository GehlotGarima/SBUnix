/* © Copyright 2015, ggehlot@cs.stonybrook.edu
	 arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
	 Everyone is permitted to copy and distribute verbatim copies
	of this license document, but changing it is not allowed.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN		1024

char path[50];
char filename[50];
char *exec_args[5];
static char pipe_buf[4096];
char PS1[50];
static int flag_pipe = 0;
static int bg_process = 0;
int parse_cmd(char *user_input);
void execute_script(char *buf);
void exec_command() {

	int pid = fork();
	
	if (pid) { //change 1
		printf("");//change 2
		if(!bg_process)
                        waitpid(pid,NULL,0);
                else {
                        bg_process = 0;
			printf("[1+] PID: %d to BG\n", pid);
		}
	} else {
		execve(filename,exec_args,NULL);
		exit(1);

	}
}

int execute_command(char cmd[5][64]){

	printf("");//change 3
	 char cur_path[50];

        if (cmd[0] == '\0')
                return 0;

        if ((strcmp(cmd[0], "/rootfs/bin/ls") == 0) || (strcmp(cmd[0],"ls") == 0) || (strcmp(cmd[0],"./ls") == 0 )) {
                strcpy(filename,"bin/ls");
                //exec_args[0] = "bin/ls";
                getcwd(cur_path,40);
                exec_args[0] = cur_path ;
		if (flag_pipe == 1)
			exec_args[1] = "wr_pipe";
		else if (flag_pipe == 2)
			exec_args[1] = "rd_pipe";
		else
			exec_args[1] = "no_pipe";

		//exec_args[2] = pipe_buf;
                exec_command();

        } else if ((strcmp(cmd[0], "/rootfs/bin/ps") == 0) || (strcmp(cmd[0],"ps") == 0) || (strcmp(cmd[0],"./ps") == 0)){
                strcpy(filename,"bin/ps");
                //exec_args[0] = "bin/ps";
                if (flag_pipe == 1)
                        exec_args[0] = "wr_pipe";
                else if (flag_pipe == 2)
                        exec_args[0] = "rd_pipe";
                else
                        exec_args[0] = "no_pipe";
    
                //exec_args[1] = pipe_buf;

                exec_command();
        } else if ((strcmp(cmd[0], "/rootfs/bin/sleep") == 0) || (strcmp(cmd[0],"sleep") == 0) || (strcmp(cmd[0],"./sleep") == 0)){
                strcpy(filename,"bin/sleep");
                //exec_args[0] = "bin/sleep";
                exec_args[0] = (char *)cmd[1];
                exec_command();

        } else if ((strcmp(cmd[0], "/rootfs/bin/alarm") == 0) || (strcmp(cmd[0],"alarm") == 0) || (strcmp(cmd[0],"./alarm") == 0)){
                strcpy(filename,"bin/alarm");
                //exec_args[0] = "bin/sleep";
                exec_args[0] = (char *)cmd[1];
                bg_process = 1;
                exec_command();

        } else if ((strcmp(cmd[0], "/rootfs/bin/echo") == 0) || (strcmp(cmd[0],"echo") == 0) || (strcmp(cmd[0],"./echo") == 0)){
                strcpy(filename,"bin/echo");
                //exec_args[0] = "bin/echo";
                if (strcmp(cmd[1],"$PATH") == 0) {
                        exec_args[0] = path;
                } else if (strcmp(cmd[1], "$PS1") == 0) {
                        exec_args[0] = PS1;
                } else { 
                        char *ptr = cmd[1];
                        if ( *ptr == '$') {
                                //printf("\n");
				exec_args[0] = "\n";
                        } else {
                               exec_args[0] =(char *) cmd[1];
                        }
                }
                if (flag_pipe == 1)
                        exec_args[1] = "wr_pipe";
                else if (flag_pipe == 2)
                        exec_args[1] = "rd_pipe";
                else
                        exec_args[1] = "no_pipe";
    
                exec_args[2] = pipe_buf;

                exec_command();

        } else if ((strcmp(cmd[0], "/rootfs/bin/kill") == 0) || (strcmp(cmd[0],"kill") == 0) || (strcmp(cmd[0],"./kill") == 0)){
                strcpy(filename,"bin/kill");
                //exec_args[0] = "bin/kill-9";
                exec_args[0] = (char *)cmd[1];
                exec_args[1] = (char *)cmd[2];
                exec_command();

        } else if (strcmp(cmd[0], "cd") == 0) {
                char *st = cmd[1];
	        char path[128] = { 0 };	
	        char *temp_cd = path;
                if (*st == ' ' || *st == '\0'){
                           strcpy(path,"/");
	
		 } else {
			if (*st != '/'){
				getcwd(path,64);
				int cd_len = strlen(path);
				temp_cd += cd_len-1;
				if (*temp_cd == '/'){
					temp_cd++;
					strcpy(temp_cd,st);
				} else {
					temp_cd++;
					*temp_cd = '/';
					 temp_cd++;
					strcpy(temp_cd,st);
				}
				//printf("path %s",path);
			 } else {
				strcpy(path,st);
			}
		}

                if (chdir(path) > 0) {
                         bzero(PS1,50);
                         char *str = PS1;
                         int ps1_size=strlen("user:~");
                         strncpy(str, "user:~", ps1_size);
                         str +=ps1_size;
                         getcwd(cur_path,40);
                         strcpy(str,cur_path);
                         str++;
                         ps1_size = strlen(PS1);
                         PS1[ps1_size] = '$';

                } else {
                        printf("No such file or directory \n");
                } 
		
        } else if (strcmp(cmd[0],"help") == 0) {
                int fd1;
		char *token;
        	fd1 = open("/rootfs/etc/help",O_RDWR);
        	char buf[2048];
        	read(fd1,buf,2048);
        	//printf("-----------HELP-----------\n");
        	token = strtok(buf,"\n");
        	while (token != NULL)
        	{  
			char *str = token;
                	while(*str == ' ' && *str) str++;
                	printf("%s \n",str);
                	token = strtok(NULL,"\n");
        	}
        } else if ((strcmp(cmd[0],"clear") == 0) || (strcmp(cmd[0],"cls") == 0)) {
                clear();
        } else if (strcmp(cmd[0],"export") == 0) {
                if (strncmp(cmd[1],"PATH=",5) == 0){
                        char *str=cmd[1];
                        str = str+5;
                        strcpy(path,str);
                } else if (strncmp(cmd[1],"PS1=",4) == 0){
                        char *str=cmd[1];
                        str = str+4;
                        strcpy(PS1,str);
			
                }
        } else if (strcmp(cmd[0],"pwd") == 0) {
                char buf[50];
                getcwd(buf,50);
                printf(" %s \n",buf);
        } else if (strcmp(cmd[0],"exit") == 0){
		exit(0);
	} else if ((strcmp(cmd[0], "/rootfs/bin/cat") == 0) || (strcmp(cmd[0],"cat") == 0) || (strcmp(cmd[0],"./cat") == 0)){
                strcpy(filename,"bin/cat");
                //exec_args[0] = "bin/cat";
		                char *st = cmd[1];
                char path[128] = { 0 };
                char *temp_cd = path;
                if (*st == ' ' || *st == '\0'){
                           strcpy(path,"/");

                 } else {
                        if (*st != '/'){
                                getcwd(path,50);
                                int cd_len = strlen(path);
                                temp_cd += cd_len-1;
                                if (*temp_cd == '/'){
                                        temp_cd++;
                                        strcpy(temp_cd,st);
                                } else {
                                        temp_cd++;
                                        *temp_cd = '/';
                                         temp_cd++;
                                        strcpy(temp_cd,st);
                                }
                                //printf("path %s",path);
                         } else {
                                strcpy(path,st);
                        }
                }

		if (cmd[1] != NULL && cmd[1] != 0 && *cmd[1] != '\0' ){
			exec_args[0] = path;
		} 
                if (flag_pipe == 1)
                        exec_args[1] = "wr_pipe";
                else if (flag_pipe == 2)
                        exec_args[1] = "rd_pipe";
                else
                        exec_args[1] = "no_pipe";


                exec_command();

        }  else if (strcmp(cmd[0],"sh") == 0){
		execute_script(cmd[1]);

	} else if (strcmp(cmd[0],"ulimit") == 0) {
                int fd1;
                char *token;
                fd1 = open("rootfs/etc/ulimit",O_RDWR);
                char buf[1024];
                read(fd1,buf,1024);
                printf("-----------ULIMIT-----------\n");
                token = strtok(buf,"\n");
                while (token != NULL)
                {  
                        char *str = token;
                        while(*str == ' ' && *str) str++;
                        printf("%s \n",str);
                        token = strtok(NULL,"\n");
                }
       } else if ( cmd[0][0] == '.' ){
		char path[50];
		char *tmp = cmd[0];
		char *tmp1 = cmd[0];
		tmp++;
			strcpy(path,"bin");
			tmp1 = path;
			tmp1 = tmp1 + strlen(path);
			strcpy(tmp1,tmp);
		strcpy(filename,path);
		exec_command();
		//printf("\n");
	//	return 1;

	}  else if (strcmp(cmd[0],"shutdown") == 0){
                shutdown();

        } else {
		char path[50];
		strcpy(path,cmd[0]); 
		if ( cmd[0][0] == '/' && cmd[0][1] == 'r' && cmd[0][2] == 'o' && cmd[0][3] == 'o' && cmd[0][4] == 't' && cmd[0][5] == 'f' && cmd[0][6] == 's' && cmd[0][7] == '/' && cmd[0][8] == 'b' && cmd[0][9] == 'i' && cmd[0][10] == 'n') {
			strcpy(filename,path);
			exec_command();
			//return 1;
		} else{
                	printf("Command cannot be recognized,type help for information \n");
		}
        }
	bzero(cmd[0],64);//change 4
	bzero(cmd[1],64);//change 5
	bzero(cmd[2],64);// change 6

	
	return 1;

}
char* ustrtok(
        char* parse_str,
        const char* delims)
{

        static char *token = NULL;
        char *str_ptr = NULL;
        int index = 0;
        int str_len = strlen(delims);

        if(!parse_str && !token)
                return NULL;

        if(parse_str && !token)
                token = parse_str;

        /*
         * skip delimiters
         */

        str_ptr = token;
        while(1) {
                for(index = 0; index < str_len; index ++) {
                        if(*str_ptr == delims[index]) {
                                str_ptr ++;
                                break;
                        }
                }

                if(index == str_len) {
                        token = str_ptr;
                        break;
                }
        }

        /*
         * End of String
         */

        if(*token == '\0') {
                token = NULL;
                return token;
        }

        while(*token != '\0') {
                for(index = 0; index < str_len; index ++) {
                        if(*token == delims[index]) {
                                *token = '\0';
                                break;
                        }
                }

                token ++;

                if (index < str_len)
                        break;
        }
        return str_ptr;
}


void execute_script(char *buffer){
        int fd1;
        char *token;
        fd1 = open(buffer,O_RDWR);
        char buf[1024];
        //char temp_cmd[5][64];
        if (fd1 == -1){
                printf("File doesnot exist\n");
                return;
        } else {
                read(fd1,buf,1024);
        }

	//printf("%s",buf);
        token = ustrtok(buf,"\n");
        if (token[0] == '#' && token[1] == '!' ) {
                token = ustrtok(NULL,"\n");
                while (token != NULL)
                        {       char *str = token;
                                while(*str == ' ' && *str) str++;
                                //printf("%s \n",str);
                                //strcpy(temp_cmd[0],str);
                                parse_cmd(str);
                                token = ustrtok(NULL,"\n");
                        }


	}
	return;
}

void execute_background(char cmd[5][64]){

        int fd1;
        char *token;
        fd1 = open(cmd[0],O_RDWR);
        char buf[1024];
        //char temp_cmd[5][64];
        if (fd1 == -1){
                printf("File doesnot exist\n");
                return;
        } else {
                read(fd1,buf,1024);
        }
        token = strtok(buf,"\n");
        if (strcmp(token,"#!/bin/sh") == 0 ) {
	//	execute_script(buf);
                token = strtok(NULL,"\n");
                while (token != NULL)
                        {       char *str = token;
                                while(*str == ' ' && *str) str++;
                                //printf("%s \n",str);
                                //strcpy(temp_cmd[0],str);
                                parse_cmd(str);
                                token = strtok(NULL,"\n");
                        }

              
        } else {
                execute_command(cmd);
        }
}

int
parse_cmd(
        char *user_input)
{
        int i=0,k=0,j=0;
        char cmd[5][64];
        int len = strlen(user_input);

       if(user_input[len-1] == '&') //change 7
                bg_process = 1;

	char *check = user_input;
	while (check && (*check == ' '))
		check++;

        if (*check == '\n' || *check == '\0' || *check == ' '){
                return 0;
        }
 

       for (k=0;k <= len;k++) {
         
        if (user_input[k] == ' ') {
                if (k != 0){
                        cmd[i][j] = '\0';
                        j=0;
                        i++;
                }
                char *newst = user_input;
                newst = newst+k+1;
                while ((*newst == ' ') && (k < len)){
                        k++;
                        newst++;
                }
		if (*newst == '|' && *(++newst) == ' ' ){
			//set_pipe_val(1);
			flag_pipe = 1;
			execute_command(cmd);
			//set_pipe_val(0);
			i=0;
			j=0;
			bzero(cmd,192);
			k = k+2;
			newst = newst+2;
	       } 
		// change 8
        } else {
                cmd[i][j] = user_input[k];
                j++;
          }

	if (i >= 5  || j >= 64){
		printf("arguments passed are out of bounds \n");
		return 0;

	}
        }

	//change 9 didnt do
        if (flag_pipe == 1)
		flag_pipe =2;

       	execute_command(cmd);

	flag_pipe = 0;

        return i;
}


/*
 * Sbush entry point
 */

int
main(
	int argc,
	char **argv,
	char **envp )
{

	int pos = 0;
	char buffer[MAX_LEN];
//	char *path = (char *)malloc(sizeof(char) * 1024);
	strcpy(path,"/rootfs/bin/");

//	int i = 0, status;
//	pid_t child_pid = -1;

	//strncpy(PS1, "[Sbush $$] >> ", strlen("[Sbush $$] >> "));
	char *str = PS1;
	int ps1_size=strlen("\n user:~");
	strncpy(str, "\n user:~", ps1_size);
	str +=ps1_size;
	getcwd(str,40);
	str++;
	ps1_size = strlen(PS1);
	PS1[ps1_size] = '$'; 

	/*get environment variables from the main's
	 * argument vector
	 */

	//get_env(envp, path);
	//environ = child_env;


	while(1) {

		//printf("\n");
		printf("%s", PS1);
		scanf("%s", buffer);

		/*
		 * parse the command line arguments
		 */

		pos = parse_cmd(buffer);
		bzero(buffer, 1024);
		bzero(exec_args, 1024);
		
		/*
		 * No command entered, next line
		 */
	        if(pos == 0) {
			continue;
		}

		}
	return 0;
}
