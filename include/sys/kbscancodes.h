
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef _KBSCANCODES_H
#define _KBSCANCODES_H

#include <sys/defs.h>

/*
 * Keys scancodes
 */

enum keyboard_scancodes {

	ESC = 0x01,
	SCHR_1,
	SCHR_2,
	SCHR_3,
	SCHR_4,
	SCHR_5,
	SCHR_6,
	SCHR_7,
	SCHR_8,
	SCHR_9,
	SCHR_0,
	UNDERSCORE_HYPHEN,
	EQUALS_PLUS,
	BCKSPC,
	TAB,
	CHR_Q,
	CHR_W,
	CHR_E,
	CHR_R,
	CHR_T,
	CHR_Y,
	CHR_U,
	CHR_I,
	CHR_O,
	CHR_P,
	CURL_BRACK_OPEN,
	CURL_BRACK_CLOSE,
	ENTER,
	LEFT_CTRL,
	CHR_A,
	CHR_S,
	CHR_D,
	CHR_F,
	CHR_G,
	CHR_H,
	CHR_J,
	CHR_K,
	CHR_L,
	COLON_SEMICLN,
	SNGL_DBL_QUOTES,
	TILDE,
	LEFT_SHIFT,
	OR_BCKSLASH,
	CHR_Z,
	CHR_X,
	CHR_C,
	CHR_V,
	CHR_B,
	CHR_N,
	CHR_M,
	COMMA_OPENDLM,
	DOT_CLOSEDLM,
	FRWDSLASH_QUESTION,
	RIGHT_SHIFT,
	PRINT_SCREEN,
	ALT,
	SPACE,
	CAPSLOCK,
	FK_F1,
	FK_F2,
	FK_F3,
	FK_F4,
	FK_F5,
	FK_F6,
	FK_F7,
	FK_F8,
	FK_F9,
	FK_F10,
	NUM_LOCK,
	SCROLL_LOCK,
	KEYPAD_HOME_7,
	KEYPAD_UP_8,
	KEYPAD_PGUP_9,
	KEYPAD_MINUS,
	KEYPAD_LFT_4,
	KEYPAD_5,
	KEYPAD_RGHT_6,
	KEYPAD_PLUS,
	KEYPAD_END_1,
	KEYPAD_DOWN_2,
	KEYPAD_PGDWN_3,
	KEYPAD_INS_0,
	KEYPAD_DOT_DEL
};


/*
 * array of characters for various keys (ACSII values)
 * scancodes above are mapped to these characters 
 */

unsigned char kbdus[128] =
{
   0,  27, '1', '2', '3', '4', '5', '6', '7', '8', 
  '9', '0', '-', '=', '\b',
  '\t', 
  'q', 'w', 'e', 'r',  
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 
    0, 
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`',   0, 
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 
  	'm', ',', '.', '/',   0,         
  '*',
    0, 
  ' ',   
    0,  
    0, 
    0,   0,   0,   0,   0,   0,   0,   0,
    0,
    0,
    0,
    0,
    0,
    0,
  '-',
    0,
    0,
    0,
  '+',
    0,
    0, 
    0,  
    0,   
    0, 
    0,   0,   0,
    0,  
    0,   
    0, 

};

/*
 * escape strings (Enter, Backspace, Tab, Escape)
 * cannot print the character with the escape character 
 */

char *escape_string[] =
{
	"\\n",
	"\\b",
	"\\t",
	"ESC"
};

#endif
