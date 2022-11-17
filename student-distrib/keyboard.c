/* keyboard.c - Functions to interact with keyboard
 */

#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"

/* Modifier keys */
#define LEFT_CTRL_PRESSED    0x1D
#define LEFT_CTRL_RELEASED   0x9D
#define LEFT_ALT_PRESSED     0x38
#define LEFT_ALT_RELEASED    0xB8
#define LEFT_SHIFT_PRESSED   0x2A
#define LEFT_SHIFT_RELEASED  0xAA
#define RIGHT_SHIFT_PRESSED  0x36
#define RIGHT_SHIFT_RELEASED 0xB6
#define CAPS_LOCK_PRESSED    0x3A
#define CAPS_LOCK_RELEASED   0xBA
#define F1_PRESSED 0x3B
#define F2_PRESSED 0x3C
#define F3_PRESSED 0x3D

/* Flags that indicate if a modifier key is pressed.
 */
uint8_t caps  = 0;
uint8_t ctrl  = 0;
uint8_t shift = 0;
uint8_t alt   = 0;

/* The table to translate scan code to ASCII code */
char scan_code_table[SCAN_CODE_NUM] = {
	'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	'\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'[', ']', '\n', '\0',
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' ,
	';', '\'', '`', '\0', '\\',
	'z', 'x', 'c', 'v', 'b', 'n', 'm',
	',', '.', '/', '\0', '\0', '\0', ' ', '\0',
};

/* The table to translate scan code to ASCII code when shift is pressed */
char scan_code_table_shift[SCAN_CODE_NUM] = {
	'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	'\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'{', '}', '\n', '\0',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' ,
	':', '\"', '~', '\0', '|',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',
	'<', '>', '?', '\0', '\0', '\0', ' ', '\0',
};

/* The table to translate scan code to ASCII code when caps is pressed */
char scan_code_table_caps[SCAN_CODE_NUM] = {
	'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	'\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'[', ']', '\n', '\0',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' ,
	';', '\'', '`', '\0', '\\',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',
	',', '.', '/', '\0', '\0', '\0', ' ', '\0',
};

/* The table to translate scan code to ASCII code when shift and caps are pressed */
char scan_code_table_shift_caps[SCAN_CODE_NUM] = {
	'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	'\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'{', '}', '\n', '\0',
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' ,
	':', '\"', '~', '\0', '|',
	'z', 'x', 'c', 'v', 'b', 'n', 'm',
	'<', '>', '?', '\0', '\0', '\0', ' ', '\0',
};

/*
*   key_init
*   initialize the keyboard's irq on PIC
*   input: None
*   output: None
*   side effect: PIC's 1st irq will be enabled. 
*/
void key_init(void) {
	/* The keyboard is connected to IR1 on the PIC */
	enable_irq(KEY_IRQ_NUM);
}


/*
*   key_handler
*   Handler function for keyboard:read what key was pressed on keyboard and print it on the screen
*   input: None
*   output: None
*   side effect: a character may appear on screen
*/
void key_handler(void) {
	cli();
	uint8_t scan_code;
	uint8_t ascii, i;
	terminal_t* term = get_current_terminal();
	if (term == NULL) {
		send_eoi(KEY_IRQ_NUM);
		sti();
		return;
	}

	/* Read from port to get the current scan code. */
	scan_code = inb(KEY_DATA_PORT);
	/* There used to be an out-of-range check here, but it was 
	 * removed because it masked out the "release" scan codes.
	 * Instead, we check for OOR cases in the "default" switch.
	 */

	/* Handle modifier keys */
	switch (scan_code) {
		case LEFT_CTRL_PRESSED:		ctrl = 1;		break;
		case LEFT_CTRL_RELEASED:	ctrl = 0;		break;
		case LEFT_ALT_PRESSED:		alt = 1;		break;
		case LEFT_ALT_RELEASED:		alt = 0;		break;
		case LEFT_SHIFT_PRESSED:	shift = 1;		break;
		case LEFT_SHIFT_RELEASED:	shift = 0;		break;
		case RIGHT_SHIFT_PRESSED:	shift = 1;		break;
		case RIGHT_SHIFT_RELEASED:	shift = 0;		break;
		case CAPS_LOCK_PRESSED:		caps = !caps;	break;
		case CAPS_LOCK_RELEASED:					break;
		case F1_PRESSED:
						printf(alt? "ALT+F1 Pressed!\n":"Only F1\n");
						terminal_switch(0);
						break;
		case F2_PRESSED:		
						printf(alt? "ALT+F2 Pressed!\n":"Only F2\n");
						terminal_switch(1);
						break;
		case F3_PRESSED:		
						printf(alt? "ALT+F3 Pressed!\n":"Only F3\n");
						terminal_switch(2);			
						break;
		default:
			if (scan_code >= SCAN_CODE_NUM) break;	/* Invalid scan code */
			if (shift && caps) {
				ascii = scan_code_table_shift_caps[scan_code];
			} else if (shift) {
				ascii = scan_code_table_shift[scan_code];
			} else if (caps) {
				ascii = scan_code_table_caps[scan_code];
			} else {
				ascii = scan_code_table[scan_code];
			}

			if (ctrl && (ascii == 'l' || ascii == 'L')) {
				terminal_clear();							/* Ctrl + L cleans the screen */
				break;
			} else if (ascii == '\n') {
				putc(ascii);
				term->kbd_buf[term->kbd_buf_count++] = '\n';
				term->readkey = 1;							/* Set the "endline" flag */
			} else if (ascii == '\b') {
				if (term->kbd_buf_count > 0) {
					putc(ascii);								/* Backspace */
					term->kbd_buf[--term->kbd_buf_count] = '\0';
				}
			} else if (ascii == '\t') {
				for (i = 0; i < 4; i++) {
					if (term->kbd_buf_count < KBD_BUF_SIZE - 1) {
						putc(' ');
						term->kbd_buf[term->kbd_buf_count++] = ' ';
					}
				}
			} else if (ascii != '\0') {
				if (term->kbd_buf_count < KBD_BUF_SIZE - 1) {
					putc(ascii);								/* Leave \b and \t to putc */
					term->kbd_buf[term->kbd_buf_count++] = ascii;
				}
			}
	}

	send_eoi(KEY_IRQ_NUM);
	// sti();
}

