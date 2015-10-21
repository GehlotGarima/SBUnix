#include <stdio.h>

static int pipe_flag ;

char temp_pipe[4096];

int get_pipe_val(){
	//printf("pipe flag returned value is %d",pipe_flag);
	return pipe_flag;
}

void set_pipe_val(int val){
	pipe_flag = val;
	//printf("pipe flag set value is %d",pipe_flag);

}

char *get_pipe_buf(){
	return temp_pipe;
}
