/* terminal.h - Defines functions and structure for terminal driver
 * vim:ts=4 noexpandtab
 */
#include "terminal.h"
#include "lib.h"
#include "keyboard.h"

terminal_t term;


/* terminal_init
 *  Initialize the terminal
 *  INPUT: None
 *  RETURN VALUE: 0 if success
 *  SIDE EFFECT: Initialize the terminal
 */
int32_t terminal_init() {
	term.readkey = 0;
	term.kbd_buf_count = 0;
	/* Clear the screen and set the cursor at
	 * the top left corner on the screen */
	terminal_clear();
	return 0;
}

/* terminal_read
 *  Read from terminal
 *  INPUT: fd     -- file descriptor, ignored
 *         buf    -- buffer to read into
 *         nbytes -- number of bytes to read
 *  RETURN VALUE: number of bytes read
 *  OUTPUT: change the contents of given buffer
 *  SIDE EFFECT: change the contents of given buffer
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
	int32_t i = 0;

	if (NULL == buf || nbytes <= 0) { return 0; }

	term.readkey = 0;
	while (1) {
		if (term.readkey != 0) {
			term.readkey = 0;
			break;
		}
	}

	/* Read from the keyboard buffer */
	/* User can only type up to 127 (KBD_BUF_SIZE - 1) characters */
	for (i = 0; i < nbytes && i < KBD_BUF_SIZE - 1; i++) {
		if (term.kbd_buf[i] == '\n') {
			break;
		}
		((char*)buf)[i] = term.kbd_buf[i];
	}
	/* Append '\n' at the end of the buffer */
	((char*)buf)[i] = '\n';
	/* Fill the rest of the buffer with 0
	 * if encounter line break */
	memset(&(((char*)buf)[i + 1]), 0, KBD_BUF_SIZE - i - 1);

	return i + 1;
}

/* terminal_write
 *  Write bytes in the given buffer to screen
 *  INPUT: fd     -- file descriptor, ignored
 *         buf    -- buffer to write from
 *         nbytes -- number of bytes to put on the screen
 *  RETURN VALUE: number of bytes to put on the screen
 *  OUTPUT: change the contents on the screen
 *  SIDE EFFECT: change the contents on the screen
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
	int32_t i = 0;
	uint8_t c = '\0';

	if (NULL == buf || nbytes <= 0) { return 0; }

	for (i = 0; i < nbytes; i++) {
		/* Cursor at the end of current line */
		if (term.cursor_x >= NUM_COLS) {
			term.cursor_y++;
			term.cursor_x = 0;
			putc('\n');
			/* Cursor at the end of the screen */
			if (term.cursor_y >= NUM_ROWS) {
				term.cursor_y = NUM_ROWS - 1;
				terminal_scroll();
			}
		}

		/* Put the character on the screen */
		c = ((char*)buf)[i];
		putc(c);

		/* Update cursor position */
		if (c == '\n' || c == '\r') {
			term.cursor_x = 0;
			term.cursor_y++;
		} else {
			term.cursor_x++;
		}
	}

	return i + 1;
}

/* terminal_open
 *  Open the terminal
 *  INPUT: filename -- filename, ignored
 *  RETURN VALUE: 0 (success)
 *  SIDE EFFECT: None
 */
int32_t terminal_open(const uint8_t* filename) {
	return 0;
}

/* terminal_close
 *  Close the terminal
 *  INPUT: fd -- file descriptor, ignored
 *  OUTPUT: close the terminal
 *  RETURN VALUE: 0 (success)
 *  SIDE EFFECT: close the terminal
 */
int32_t terminal_close(int32_t fd) {
	term.kbd_buf_count = 0;
	return 0;
}

/* terminal_scroll
 *  Scroll the screen up by one line
 *  INPUT: none
 *  OUTPUT: change the contents on the screen
 *  RETURN VALUE: none
 *  SIDE EFFECT: change the contents on the screen
 */
void terminal_scroll() {
	int i;
	for (i = 1; i < NUM_ROWS; i++) {
		/* Each character occupies 2 bytes */
		memcpy((uint8_t*)(VIDEO + (i - 1) * NUM_COLS * 2),
			   (uint8_t*)(VIDEO + i * NUM_COLS * 2),
			   NUM_COLS * 2);
	}
	/* Clear the last line */
	memset((uint8_t*)(VIDEO + (NUM_ROWS - 1) * NUM_COLS * 2), 0, NUM_COLS * 2);
}

/* terminal_clear
 *  Clear the screen and reset the cursor position
 *  INPUT: none
 *  OUTPUT: Clear the screen
 *  RETURN VALUE: none
 *  SIDE EFFECT: Clear the screen
 */
void terminal_clear() {
	memset((uint8_t*)(VIDEO), 0, NUM_COLS * NUM_ROWS * 2);
	term.cursor_x = term.cursor_y = 0;
	set_cursor(0, 0);
}

/* get_current_terminal
 *  Get the pointer to current terminal
 *  INPUT: none
 *  RETURN VALUE: pointer to current terminal
 *  OUTPUT: none
 *  SIDE EFFECT: none
 */
terminal_t* get_current_terminal() {
	return &term;
}
