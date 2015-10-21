/* © Copyright 2015, ggehlot@cs.stonybrook.edu
	 arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
	 Everyone is permitted to copy and distribute verbatim copies
	of this license document, but changing it is not allowed.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

char print_buf[1024];
int
print_string(const char *s, char *str,int index_main)
{
	int i, len;
	len = strlen(s);

	for(i = 0; i < len; i++)
		str[index_main++] = *s++;

	return i; 
}

int
print_int(int num, char *str , int index_main)
{
	char s[10], temp;
	int index = 0, j = 0;
	int sign = 0;
	if (num < 0){
		sign  = 1;
		num = (-1)*num;
	}

	bzero(s, 10);
	do {
        	s[index++] = '0' + (num%10);
        	num/=10;
	} while (num != 0);

	for (j = 0; j < index/2; j++) {
        	temp = s[j];
        	s[j] = s[index-1-j];
        	s[index-1-j] = temp;
	}
	if(sign == 1)
             str[index_main++] = '-';

	for(j = 0; s[j] ; j++)
	{
		str[index_main++] = s[j]; 
        }
	return j+sign; 
}

int
print_hex(int addr, char *str,int index_main) {
	char arr[33]; 
	char *ptr = &arr[sizeof(arr)-1]; 
	*ptr = '\0'; 
	int count =0;

	while(addr!=0)
	{ 
		*--ptr="0123456789abcdef"[addr%16]; 
		addr/=16;
	}

	while(*ptr){
		str[index_main++] = *ptr++;
		count +=1; 
	}

	return count;
}

int
print_pointer(uint64_t addr, char *str,int index_main) {
        char arr[33];
	
        char *ptr = &arr[sizeof(arr)-1];
        *ptr = '\0';
        int count =0;

        while(addr!=0)
        {
                *--ptr="0123456789ABCDEF"[addr%16];
                addr/=16;
        }
	*--ptr ='x';
	*--ptr ='0';

        while(*ptr){
                str[index_main++] = *ptr++;
                count +=1;
        }

        return count;
}

int
print_char(int ch, char *str,int index_main) {

	str[index_main++] = ch;

	return 1;
}

static int
vprintf(
	const char *format,
	va_list args)
{
	int printed = 0;
//	char print_buf[1024];
//	char *str = print_buf;
	int index=0;
	int ret=0;
        bzero(print_buf,1024);	
	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;

			if (*format == '\0') break;


			switch(*format) {

			case 's':
				ret = print_string(va_arg(args, char *), print_buf,index);
				index = index+ret;
				printed++;
				break;

			case 'd':
				ret=print_int(va_arg(args, int), print_buf,index);
				index = index + ret;
				printed++;
				break;

			case 'c':
				print_char(va_arg(args, int), print_buf,index);
				index++;
				printed++;
				break;

			case 'x':
				ret = print_hex(va_arg(args, int), print_buf,index);
				index = index+ret;
				printed++;
				break;

                        case 'p':
                                ret = print_pointer(va_arg(args, uint64_t), print_buf,index);
                                index = index+ret;
                                printed++;
                                break;

			}
		} else {
			print_buf[index++] = *format;	
		}
	}
//	str++;
	print_buf[index] = '\0';
	write(1, print_buf, strlen(print_buf));
	return 0;
}

int printf(const char *format, ...) {

	va_list val;

	va_start(val, format);

	/*
	while(*format) {
		//write(1, format, 1);
		++printed;
		++format;
	}*/

	return vprintf(format, val);

}
