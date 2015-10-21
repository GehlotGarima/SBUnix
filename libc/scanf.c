/* © Copyright 2015, ggehlot@cs.stonybrook.edu
	 arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
	 Everyone is permitted to copy and distribute verbatim copies
	of this license document, but changing it is not allowed.
*/
#include<stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

int scan_string(char *str) {

	int i;
	i= read(0,str, 1024);
	return i;
}

int scan_integer(int *str) {
	int pos = 0;
	char buffer[10] = {'\0'};
	read(0,buffer,sizeof(int));
	while ((buffer[pos] != '\0') && (buffer[pos] != '\n' ) ){
	pos++;
	}
	pos--;
	int val = 0;
	int io =1;
	while (pos >= 0) {
		buffer[pos] = buffer[pos] - 48;
		val = val + buffer[pos]*io;
		pos--;
		io= io*10;
	}
		
	*str = val;
	return val;
}

int scan_hexadecimal(int *str) {
	 int pos = 0;
        char buffer[10] = {'\0'};
        read(0,buffer,sizeof(int));
        while ((buffer[pos] != '\0') && (buffer[pos] != '\n' ) ){
        	pos++;
        }
        pos--;
        int val = 0;
        int io =1;
        while (pos >= 0) {
		if (buffer[pos] >= 97) {
			buffer[pos] = buffer[pos] - 87;
		} else {
                	buffer[pos] = buffer[pos] - 48;
		}
                val = val + buffer[pos]*io;
                pos--;
                io= io*16;
        }

        *str = val;
        return val;
}

int scan_char(char *str) {

	int i;
	
	i = read(0,str,sizeof(str));
	return i;

}

int vsscanf(const char *format, va_list list)

{
	int scanned = 0;
	char *c;
	char *ch;
	int *num = NULL;

	for(; *format != '\0'; format++ ){

		while (*format == ' ')
			format++;

		if(*format == '%'){
			++format;
		

		if (*format == '\0')
			break;

		switch (*format)
		{

			case 's' :
				ch = (char *) va_arg(list,char *);
				scan_string(ch);
				scanned++;
				break;
			case 'd' :
				num = (int *) va_arg(list,int * );
				scan_integer(num);
				scanned++;
				break;
			case 'x' :
				scan_hexadecimal(va_arg(list, int *));
				scanned++;
				break;
			case 'c' :
				c = (char *)va_arg(list,char *);
				scan_char(c);
				scanned++;
				break;
		}
		} else {
			format++;
		}
	}
	return scanned;
}
	
int scanf(const char *format,...) {

	int cnt;
	va_list list;
	
	va_start(list,format);

	cnt = vsscanf(format,list);

	va_end(list);

	return cnt;

}
