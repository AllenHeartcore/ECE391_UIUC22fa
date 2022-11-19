/* terminal.h - Defines functions and structure for terminal driver
 * vim:ts=4 noexpandtab
 */
#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
#include "page.h"

uint8_t* backup_buf_add[3] = {(uint8_t*)0xBA000, (uint8_t*)0xBB000, (uint8_t*)0xBC000}; // Store backup buffers' address
terminal_t terms[TERM_NUM]; // array for three terminals
volatile uint8_t	current_term_id; // pointing out which is currently displayed terminal

/* terminal_init
 *  Initialize the terminal
 *  INPUT: None
 *  RETURN VALUE: 0 if success
 *  SIDE EFFECT: Initialize the terminal
 */
int32_t terminal_init() {
	int i;
	for(i = 0; i<3; i++){
		terms[i].readkey = 0;
		terms[i].kbd_buf_count = 0;
		terms[i].cursor_x = 0;
		terms[i].cursor_y = 0;
		vga_enable_cursor(0, 14);					/* Set cursor shape */
		printf("init terminal");					/* debug */
		vga_redraw_cursor(terms[i].cursor_x, terms[i].cursor_y);	/* Set initial cursor position */
	}
	current_term_id = 0; /* Set terminal0 as the first terminal displayed */
	return 0;
}

/* terminal_switch
 *  Switch the terminal
 *  INPUT: term_id -- which terminal is switched to
 *  RETURN VALUE: none
 *  OUTPUT: display of VGA will change
 *  SIDE EFFECT: change the video memory's content and backup buffer's content
 */
void terminal_switch(uint8_t term_id){
	/* Sanity check */
	if(term_id > 2 || term_id == current_term_id)
		return;
	/* Backup current_term */
	remap_vidmap_page(current_term_id);
	memcpy((void*)backup_buf_add[current_term_id],(void*)VIDEO, VIDEO_PAGE_SIZE );

	/* Load switch term's vdieo memory */
	memcpy((void*)VIDEO, (void*)backup_buf_add[term_id], VIDEO_PAGE_SIZE );

	/* Update cursor */
	vga_redraw_cursor(terms[term_id].cursor_x, terms[term_id].cursor_y);            // Redraw the new one

	/* Update current_term_id */
	current_term_id = term_id;

	/* Remap user video memory */
	remap_vidmap_page(current_term_id);
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

	memset(terms[cur_sch_index].kbd_buf, 0, KBD_BUF_SIZE);	/* Clear the buffer */
	terms[cur_sch_index].kbd_buf_count = 0;					/* Reset the buffer count */
	terms[cur_sch_index].readkey = 0;						/* Reset the "endline" flag */
	while (!terms[cur_sch_index].readkey);					/* Wait on the flag */
	// printf("The terminal is %d\n",current_term_id);
	/* Read from the keyboard buffer */
	/* User can only type up to 127 (KBD_BUF_SIZE - 1) characters */

	/* In 3 conditions will the read/write loop be broken:
	 *     1. If more than 'nbytes' are read/written, or
	 *     2. if a NUL is encountered, or
	 *     3. if we already read 128 (KBD_BUF_SIZE) bytes. */
	for (i = 0; i < nbytes && i < KBD_BUF_SIZE && terms[cur_sch_index].kbd_buf[i] != '\0'; i++) {
		((char*)buf)[i] = terms[cur_sch_index].kbd_buf[i];
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
	/* See above for loop-breaking conditions */
	for (i = 0; i < nbytes && ((char*)buf)[i] != '\0'; i++) {
			putc(((char*)buf)[i]);
	}

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
	terms[cur_sch_index].kbd_buf_count = 0;
	return 0;
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
}

/* get_current_terminal
 *  Get the pointer to currently displayed terminal
 *  INPUT: none
 *  RETURN VALUE: pointer to current terminal
 *  OUTPUT: none
 *  SIDE EFFECT: none
 */
terminal_t* get_current_terminal() {
	return &(terms[current_term_id]);
}

int32_t illegal_open(const uint8_t* filename) {
	printf("WARNING: You cannot open the terminal.\n");
	return -1;
}

int32_t illegal_close(int32_t fd) {
	// printf("WARNING: You cannot close the terminal.\n");
	/* Otherwise, the message will pop up 
	 * each time we halt a new shell (excl. #0) */
	return -1;
}

int32_t illegal_read(int32_t fd, void* buf, int32_t nbyte){
    printf("WARNING: You cannot read in stdout.\n");
    return -1;
}

int32_t illegal_write(int32_t fd, const void* buf, int32_t nbyte){
    printf("WARNING: You cannot write in stdin.\n");
    return -1;
}
