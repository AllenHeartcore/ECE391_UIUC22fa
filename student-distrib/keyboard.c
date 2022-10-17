/* keyboard.c - Functions to interact with keyboard
 */

#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

/* Some special keys. We can modified ASCII later for ckpt2 or ckpt3 */
#define BACKSPACE 0x0
#define TAB 0x0
#define ENTER 0x0
#define LEFT_CONTROL 0x0
#define LEFT_ALT 0x0
#define LEFT_SHIFT 0x0
#define RIGHT_SHIFT 0x0
#define CAPS 0x0

/* Flags that indicate if a modifier key is pressed */
uint8_t caps   = 0;
uint8_t ctrll  = 0;
uint8_t ctrlr  = 0;
uint8_t shiftl = 0;
uint8_t shiftr = 0;
uint8_t altl   = 0;
uint8_t altr   = 0;

/* The table to translate scan code to ASCII code */
char scan_code_table[SCAN_CODE_NUM] = {
	'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	BACKSPACE, TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'[', ']', ENTER, LEFT_CONTROL,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' ,
	';', '\'', '`', LEFT_SHIFT, '\\',
	'z', 'x', 'c', 'v', 'b', 'n', 'm',
	',', '.', '/', RIGHT_SHIFT, '\0', LEFT_ALT, ' ', CAPS,
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
	uint8_t scan_code;
	uint8_t ascii;
	cli();
	/* Read from port to get the current scan code. */
	scan_code = inb(KEY_DATA_PORT);
	if (scan_code >= SCAN_CODE_NUM) {
		send_eoi(KEY_IRQ_NUM);
		return;
	}
	ascii = scan_code_table[scan_code];

	/* Ignore special keys for now */
	if (ascii == 0) {
		send_eoi(KEY_IRQ_NUM);
		return;
	}

	// putc(ascii);
	printf("%c", ascii);
	send_eoi(KEY_IRQ_NUM);
	sti();
}

