#ifndef _PIPE_H
#define _PIPE_H

char temp_pipe[4096];
int get_pipe_val();
void set_pipe_val(int val);
char *get_pipe_buf();

#endif

