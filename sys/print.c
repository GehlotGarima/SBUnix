
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include <sys/sbunix.h>
#include <sys/gdt.h>
#include <sys/tarfs.h>
#include <stdarg.h>
#include <sys/interrupts.h>
#include <sys/string.h>
#include <sys/defs.h>

int check_print_escape(const char *format); /*function to execute backspace*/
int print_to_console(const char *string, int size);/*function to print console*/
void update_linefeed();/*keeps track of cursor position*/
void clear_console();/*function to clear console*/
static int vprintf(const char *format,va_list args);/*function to excute actual functionality of vprintf*/
void scroller(int no_of_lines);


static char *v_ptr = (char *)0xFFFFFFFF800B8000;
static int line_no=24;
static int column_no=80;
//static int stat = 0;

char *starting_address_of_VGA_buffer = (char *)0xFFFFFFFF800B8000;
char *end_address_of_VGA_buffer = (char *)0xFFFFFFFF800B8F00UL;

uint64_t get_VGA_address()
{
	return (uint64_t)v_ptr;
}

void set_VGA_address(uint64_t newadd)
{

	v_ptr = (char *)newadd;

}

int find_row_no()
{
	return (((uint64_t)v_ptr - (uint64_t)starting_address_of_VGA_buffer)/160);

}

int find_column_no()
{
	return ((((uint64_t)v_ptr - (uint64_t)starting_address_of_VGA_buffer)/2)%80);
}

void set_print_location(int line, int column)
{
	uint64_t newadd;
	newadd = (uint64_t)starting_address_of_VGA_buffer +(line*160 + column*2);
	set_VGA_address(newadd);

}

void cursor_print()
{
	uint16_t cursor;
	cursor = (find_row_no())*80 + find_column_no();
	outb(0x3D4, 14);
	outb(0x3D5, cursor >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, cursor);


} 
void scroller(int no_lines){

        uint64_t source = (uint64_t)starting_address_of_VGA_buffer + no_lines*160;
        uint64_t destination = (uint64_t)starting_address_of_VGA_buffer;
        uint64_t num = (24-no_lines)*160;
        memcpy((void *)destination,(void *)source,num);

        source  = (uint64_t)starting_address_of_VGA_buffer+num;
        num = no_lines*160;
        memset((void *)source,0,num);
        int line_no = find_row_no();
        int col_no  = find_column_no();
        line_no = line_no - no_lines;
        set_print_location(line_no,col_no);


}
void check_scroll()
{
        int row_no = find_row_no();
        int col_no = find_column_no();
        if (row_no >=  23 && col_no < 30 ) {
                scroller(1);
        }

}
int print_to_console(const char *string, int size)
{
        int count = 0;
        while (*string && count < size) {
                check_scroll();
                *v_ptr = *string; /*sending output to video memory*/
                ++string;
                v_ptr +=2;
                count++;

        }

        return 1;
}
int print_char1(char newchar)
{

	check_scroll();
	*v_ptr = newchar;
	v_ptr += 2;
	return 1; 



}
void print_newline()
{
	int line_no = find_row_no() + 1;
	set_print_location(line_no,0);
	check_scroll();

}

void print_backspace()
{      
	if (get_VGA_address() >= (uint64_t)starting_address_of_VGA_buffer) 
	{       
                char *ch; 
		ch = (char *)(get_VGA_address() - 2);
		if (*ch == ' ' && *(ch - 2) == ' ' && *(ch - 4) == ' ' &&  *(ch - 6)== ' '){
	                 set_VGA_address(get_VGA_address() - 10);
       		         print_char1(' ');
               		 set_VGA_address(get_VGA_address() - 2);

		} else { 
        		 set_VGA_address(get_VGA_address()-2);
			 print_char1(' ');
			 set_VGA_address(get_VGA_address()-2);
		}
	}

}
	
void print_tab()
{

	set_VGA_address(get_VGA_address()+8);
	check_scroll();

}
int check_print_escape(const char *format){
        if (*format == '\n' )
        {
		print_newline();
                return 1;
        } else if (*format == '\b')
	{
		print_backspace();
                return 1;
	} else if (*format == '\t')
	{
		print_tab();
		return 1;
	} else
		return 0;

}


/*for printing string*/
int print_string(char *str)
{
        int size = strlen(str);
        print_to_console(str,size);
        return 1;
}

/*for printing integer*/
int print_int(int num)
{
        char str[10], temp;
        int index = 0, j = 0;
        char *t = str;
        int n=5;
        while (n != 0) {
                *t++ = 0;
                n--;
        }
       if (num == 0){
                str[index] = '0';
        }
	while (num > 0) {
                str[index++] = '0' + (num%10);
                num/=10;
        }

        for (j = 0; j < index/2; j++) {
                temp = str[j];
                str[j] = str[index-1-j];
                str[index-1-j] = temp;
        }

        int size = strlen(str);

        print_to_console((const char *)str,size);
        return 1;
}

/*for printing hex*/
int print_hex(int addr) {
        char arr[33];
        char *ptr = &arr[sizeof(arr)-1];
        *ptr = '\0';
       int count = 0;
        while( addr != 0 )
        {
                *--ptr="0123456789abcdef"[addr%16];
                addr/=16;
                count++;
        }

        print_to_console(ptr,count);
        return 1;
}

/*for printing pointer*/
int print_ptr(void *addr_main)
{
        print_string("0x");
        char arr[33];
        unsigned long long addr = (unsigned long long ) addr_main;
        char *ptr = &arr[sizeof(arr)-1];
        *ptr = '\0';
       int count = 0;
        while( addr != 0 )
        {
                *--ptr="0123456789abcdef"[addr%16];
                addr/=16;
                count++;
        }

	print_to_console(ptr,count);
        return 1;

}


/*for printing char*/
int print_char(char ch) {
        char str[2];
        str[0] = ch;
        str[1] = '\0';
        int size = strlen(str);
        print_to_console(str,size);
        return 1;
}


/*for printing line at particular coordinate*/
int print_line(int row,int column,const char *string, ...) {


        char *ptr = v_ptr;
        v_ptr = starting_address_of_VGA_buffer+(2*80*column+row*2); 

        va_list val;
        va_start(val, string);


        vprintf(string,val);
        va_end(val);

        v_ptr = ptr;
        return 1;


}

/*for function of backspace*/
int check_back_space(const char *format) {
        char *sp =NULL;
        int x = 2;
        if (*format == '\b' ) {
                v_ptr= v_ptr-x;
                *v_ptr= *sp;

                if (column_no == 0) {
                         column_no = 79;
                        line_no--;
                         } else {
				column_no--;
                        }

                return 1;
        } else {

                return 0;
        }

}


/*for clearing console*/
void clear_console() {
        char *v_ptr1 = (char *)0xFFFFFFFF800B8000;
        int count =0;
	uint8_t color=0;
//	color = ((4 << 4) & 0xF0) | (0 & 0x0F);
        while (count < (80*24*2)) {

        *(v_ptr1+count) = ' ';
	*(v_ptr1+count+1) = color;
        count= count+2;
        }
	v_ptr = v_ptr1;


}

/*for executing printf*/
static int  vprintf(const char *format,va_list args)
{
        int printed = 0;
        const char *ch;
        //int size;
        int ret;
       while (*format != 0) {

                if (*format == '%') {
                        ch = format;
                        ++format;

                        if (*format == '\0'){
                                break;
                        }
                        switch(*format) {
                        case 's':
                                ret = print_string(va_arg(args, char *));
                                printed=printed+ret;
                                break;

                        case 'd':
                                print_int(va_arg(args, int));
 				printed++;
                                break;

                        case 'c':
                                print_char(va_arg(args, int));
                                printed++;
                                break;

                        case 'x':
                                print_hex(va_arg(args, int));
                                printed++;
                                break;

                        case 'p':
                                print_ptr(va_arg(args,void *));
                                printed++;
                                break;

                        default :
                                print_to_console(ch,1);
                                print_to_console(format,1);
                                printed++;
                        }
                } else {
                        format = format + check_print_escape(format) + check_back_space(format);
			if (*format != '\0'){ 
                        	print_to_console(format,1);
			} else {
				break;
			}
			printed++;
                }

                format++;
        }
	return printed;
}


/*printf implementation*/
int printf(const char *format, ...) {
        va_list val;
        va_start(val, format);
        int ret = vprintf(format,val);
        va_end(val);
	return ret;
}
/*function to print newline
void print_newline() {
                v_ptr = v_ptr+ (160 - ((v_ptr - (char *)0xFFFFFFFF800B8000)%160));
		//v_ptr = v_ptr + (160 - column_no*2) ;
			
}
*/

/*function to print backspace
void print_backspace() {
        v_ptr = v_ptr - 2;
        *v_ptr = ' ';
        v_ptr=v_ptr-2;


       // char *sp =NULL;
        //      *sp = ' ';
        int x=2;

        if (stat == 0) {
                char *c1;
        char *c2;
              c1 = (char *)(v_ptr-2);
        c2 = (char *)(v_ptr-4);
                *v_ptr = ' ';
                v_ptr= v_ptr - x;
        *v_ptr =' ';
        v_ptr = v_ptr - 2;
        print_to_console(c2,1);
                        print_to_console(c1,1);
                stat=1;
        } else {
                *v_ptr = ' ';
                v_ptr= v_ptr - x;
        *v_ptr =' ';
        v_ptr = v_ptr - 2;

        } 
                if (column_no == 0) {
                         column_no = 79;
                        line_no--;
                         } else {
                        column_no--;
                        }


} */

int32_t puts(char* mystring)
{
//    char *temp = mystring;
    int32_t len = 0;
/*    for (; *temp; temp++) {
        *v_ptr = *temp;
	v_ptr += 2;
        len++;
    }
//	v_ptr += 2; */
	len =printf(mystring);
    return len;
}
