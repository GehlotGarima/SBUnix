#ifndef __SBUNIX_H
#define __SBUNIX_H

#include <sys/defs.h>

int printf(const char *fmt, ...);

void print_newline();
void print_backspace();
void clear_console();
int print_line(int row,int column,const char *string, ...);
int32_t puts(char* mystring);
void print_backspace();
void print_tab();
int scanf_stdin(void *addr, int num);
uint64_t get_VGA_address();
void cursor_print();
#endif
