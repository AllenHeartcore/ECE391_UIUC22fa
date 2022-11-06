/* terminal.h - Defines functions and structure for terminal driver
 * vim:ts=4 noexpandtab
 */
#include "types.h"

#define KBD_BUF_SIZE 128

/* Terminal struct, used to keep all informations related to a terminal */
typedef struct terminal_t {
	volatile uint8_t readkey;   /* enable/disable reading from kbd buffer   */
	uint8_t  cursor_x;          /* column number of current cursor position */
	uint8_t  cursor_y;          /* row number of current cursor position    */
	uint8_t  kbd_buf[KBD_BUF_SIZE];     /* keyboard buffer                  */
	uint8_t  kbd_buf_count;     /* number of characters in keyboard buffer  */
} terminal_t;

/* Initialize the terminal */
extern int32_t terminal_init();
/* Read from a terminal */
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
/* Write to a terminal */
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
/* Open a terminal */
extern int32_t terminal_open(const uint8_t* filename);
/* Close a terminal */
extern int32_t terminal_close(int32_t fd);
/* Scroll the terminal */
extern void terminal_scroll();
/* Clear the screen and reset the cursor position */
extern void terminal_clear();
/* Get current terminal */
extern terminal_t* get_current_terminal();
/* terminal_open fault wrapper */
extern int32_t illegal_open(const uint8_t* filename);
/* terminal_close fault wrapper */
extern int32_t illegal_close(int32_t fd);
/* stdin_read fault wrapper */
extern int32_t illegal_read(int32_t fd, void* buf, int32_t nbyte);
/* stdout_write fault wrapper */
extern int32_t illegal_write(int32_t fd, const void* buf, int32_t nbyte);
