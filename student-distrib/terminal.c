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
	get_cursor(&term.cursor_x, &term.cursor_y);
	vga_enable_cursor(0, 14);							/* Set cursor shape */
	vga_redraw_cursor(term.cursor_x, term.cursor_y);	/* Set cursor position */
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

	memset(term.kbd_buf, 0, KBD_BUF_SIZE);	/* Clear the buffer */
	term.kbd_buf_count = 0;					/* Reset the buffer count */
	term.readkey = 0;						/* Reset the "endline" flag */
	while (1) {
		if (term.readkey != 0) {
			term.readkey = 0;
			break;
		}
	}

	/* Read from the keyboard buffer */
	/* User can only type up to 127 (KBD_BUF_SIZE - 1) characters */
	for (i = 0; i < nbytes && i < KBD_BUF_SIZE; i++) {
		((char*)buf)[i] = term.kbd_buf[i];
	}
	/* Fill the rest of the buffer with 0 */
	memset(&(((char*)buf)[i]), 0, KBD_BUF_SIZE - i);

	return i;
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

	if (NULL == buf || nbytes <= 0) { return 0; }

	/* Write to the screen */
	for (i = 0; i < nbytes; i++) {
		putc(((char*)buf)[i]);
	}

	/* Update cursor position */
	get_cursor(&term.cursor_x, &term.cursor_y);

	return i;
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
	scroll();
	get_cursor(&term.cursor_x, &term.cursor_y);
}

/* terminal_clear
 *  Clear the screen and reset the cursor position
 *  INPUT: none
 *  OUTPUT: Clear the screen
 *  RETURN VALUE: none
 *  SIDE EFFECT: Clear the screen
 */
void terminal_clear() {
	clear();
	get_cursor(&term.cursor_x, &term.cursor_y);
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
