
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include <sys/interrupts.h>
#include <sys/kbscancodes.h>
#include <sys/sbunix.h>
#include <sys/string.h>

static int shift;
static volatile char scan_buf[1024];
static volatile int scan_len = 0;
static volatile int scan_flag = 0; 

int  scanf_stdin(void *addr,int count) {

   scan_flag = 1;
  	__asm__ __volatile__("sti;"); 
	
   //uint64_t vga_addr = get_VGA_address();
	scan_len = 0;
	
    while ( scan_flag == 1){

	cursor_print();
    }
	if (count <  scan_len)
		scan_len = count;
	
    memcpy((void *)addr,(void *)scan_buf,scan_len);
	while(scan_len >=0){
	scan_buf[scan_len]= '\0';
	--scan_len;
	}
		
	
	int len = scan_len;
	scan_len = 0;
	return len;

}
void 
interpret_scancode(uint16_t scan_code)
{
	char interpreted_char = '\0';
	int esc_index;

	//space
	if(scan_code == SPACE)
		interpreted_char = kbdus[scan_code];

	//enter
	if(scan_code == ENTER){
		interpreted_char = kbdus[scan_code];
		esc_index = 0;
		if (scan_flag == 1) {
			scan_flag =0;
		}
		print_newline();	
	}

	//backspace
	if(scan_code == BCKSPC){
		interpreted_char = kbdus[scan_code];
		esc_index = 1;
                if (scan_flag == 1 &&  scan_len == 0){
                      return;
                } else {
                                print_backspace();
                }
                if ((scan_flag == 1) && (scan_len > 0))
                {
                scan_buf[--scan_len] = ' ';
                }


	}

	//ESC
	if(scan_code == ESC){
		interpreted_char = kbdus[scan_code];
		esc_index = 3;
	}

	//Tab
	if(scan_code == TAB) {
		interpreted_char = kbdus[scan_code];
		esc_index = 2;
		print_tab();
	}	

	print_line(43, 24,"   ");

	//for lowercase and upper case alphabets
	if((scan_code >= CHR_Q && scan_code <= CHR_P) ||
	   (scan_code >= CHR_A && scan_code <= CHR_L) ||
	   (scan_code >= CHR_Z && scan_code <= CHR_M)) {

		interpreted_char = kbdus[scan_code] + (shift>0 ? (-32) : 0);
		printf("%c", interpreted_char);
		print_line(43, 24, "%c", interpreted_char);
		if (scan_flag == 1)
                {
                        scan_buf[scan_len++] = interpreted_char;
                }

		return;
	}

	//shift keys and special characters
	if(shift) {
		if(scan_code >= SCHR_1 && scan_code <= SCHR_0) 
				interpreted_char = "!@#$%^&*()"[scan_code-SCHR_1];

		if(scan_code >= CURL_BRACK_OPEN && scan_code <= CURL_BRACK_CLOSE) 
				interpreted_char = "{}"[scan_code-CURL_BRACK_OPEN];

		if(scan_code >= COMMA_OPENDLM && scan_code <= FRWDSLASH_QUESTION) 
				interpreted_char = "<>?"[scan_code-COMMA_OPENDLM];

		if(scan_code >= COLON_SEMICLN && scan_code <= TILDE) 
                	        interpreted_char = ":\"~"[scan_code-COLON_SEMICLN];

		if(scan_code >=  UNDERSCORE_HYPHEN && scan_code <= EQUALS_PLUS) 
				interpreted_char = "_+"[scan_code-UNDERSCORE_HYPHEN];

		if(scan_code == OR_BCKSLASH) 
			interpreted_char = '|';
	} else {
		interpreted_char = kbdus[scan_code];
	}

	//Escape characters
	if((interpreted_char == 8) || (interpreted_char == 9) ||
		(interpreted_char == 10) || (interpreted_char == 27)) {
		print_line(43, 24, "%s", escape_string[esc_index]);
		return;	
	}

	//printable characters
	if(interpreted_char) {
		if (scan_flag == 1)
		{
			scan_buf[scan_len++] = interpreted_char;
		}
		printf("%c", interpreted_char);
		print_line(43, 24, "%c", interpreted_char);

	}
}

void
keyboard_isr() {

	uint16_t scancode = 0;

	//Read the status
	uint16_t status = inb(0x64);

	/*
	 * check if we can read from the buffer,
	 * lowest bits are set if the buffer is not empty
	 */

	if(status & 0x01) {

		// Read scan from port 0x60
		if((scancode = inb(0x60)) < 0) {
			return;
		} else {
			if(scancode & 0x80) {
				//key released
				scancode = (shift & (1<<6) ? scancode : scancode & 0x7F);
				shift = 0;
				return;
			}
			else {
				//check for the shift key
				if(scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT) {
					shift = 1;
				}
			}
		}

		/*
	 	 * interpret scancode (convert to ASCII)
	 	 */
		
		interpret_scancode(scancode);
	}

	return;
}

