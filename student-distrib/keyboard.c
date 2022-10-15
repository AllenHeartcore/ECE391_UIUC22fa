/* keyboard.c - Functions to interact with keyboard
 */

#include "keyboard.h"
#include "lib.h"

/* Some special keys. We can modified ASCII later for ckpt2 or ckpt3 */
#define BACKSPACE 0x0
#define TAB 0x0
#define ENTER 0x0
#define LEFT_CONTROL 0x0
#define LEFT_ALT 0x0
#define LEFT_SHIFT 0x0
#define RIGHT_SHIFT 0x0
#define CAPS 0x0

/* The table to translate scan code to ASCII code */
char scan_code_table[SCAN_CODE_NUM][2]={
    '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', BACKSPACE, TAB,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', ENTER, LEFT_CONTROL, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', LEFT_SHIFT, '\\', 'z', 'x', 'c', 'v', 
	'b', 'n', 'm',',', '.', '/', RIGHT_SHIFT,'\0', LEFT_ALT, ' ', CAPS
};
/* Initialize keyboard */
void key_init(void) {
}

/* Handler function for keyboard*/
void key_handler(void) {
}

