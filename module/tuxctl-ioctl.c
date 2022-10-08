/* tuxctl-ioctl.c
 *
 * Driver (skeleton) for the mp2 tuxcontrollers for ECE391 at UIUC.
 *
 * Mark Murphy 2006
 * Andrew Ofisher 2007
 * Steve Lumetta 12-13 Sep 2009
 * Puskar Naha 2013
 */

#include <asm/current.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/miscdevice.h>
#include <linux/kdev_t.h>
#include <linux/tty.h>
#include <linux/spinlock.h>

#include "tuxctl-ld.h"
#include "tuxctl-ioctl.h"
#include "mtcp.h"

#define debug(str, ...) \
	printk(KERN_DEBUG "%s: " str, __FUNCTION__, ## __VA_ARGS__)

static uint8_t acknowledged = 1;
static unsigned long* arg_store;
unsigned char LEDS[16] = {0b11100111, 0b00000110, 0b11001011, 0b10001111,\
						  0b00101110, 0b10101101, 0b11101101, 0b10100110,\
						  0b11101111, 0b10101111, 0b11101110, 0b01101101,\
						  0b11100001, 0b01001111, 0b11101001, 0b11101000};

/************************ Protocol Implementation *************************/

/* tuxctl_handle_packet()
 * IMPORTANT : Read the header for tuxctl_ldisc_data_callback() in 
 * tuxctl-ld.c. It calls this function, so all warnings there apply 
 * here as well.
 */
void tuxctl_handle_packet (struct tty_struct* tty, unsigned char* cmd)
{
    unsigned a, b, c;

    a = cmd[0]; /* Avoid printk() sign extending the 8-bit */
    b = cmd[1]; /* values when printing them. */
    c = cmd[2];

	switch (a) {
	case MTCP_POLL_OK:
		* arg_store = (b & 0x0F) \				// _ _ _ _ C B A S
					| ((c & 0x09) << 4) \		// > _ _ ^ C B A S
					| ((c & 0x02) << 6) \		// > < _ ^ C B A S
					| ((c & 0x04) << 5);		// > < v ^ C B A S
	case MTCP_ACK:								// ctrl flow continues
		acknowledged = 1;						// "responded"
		return;
	default:
		return -EINVAL;
	}
}

/******** IMPORTANT NOTE: READ THIS BEFORE IMPLEMENTING THE IOCTLS ************
 *                                                                            *
 * The ioctls should not spend any time waiting for responses to the commands *
 * they send to the controller. The data is sent over the serial line at      *
 * 9600 BAUD. At this rate, a byte takes approximately 1 millisecond to       *
 * transmit; this means that there will be about 9 milliseconds between       *
 * the time you request that the low-level serial driver send the             *
 * 6-byte SET_LEDS cmd and the time the 3-byte ACK cmd finishes               *
 * arriving. This is far too long a time for a system call to take. The       *
 * ioctls should return immediately with success if their parameters are      *
 * valid.                                                                     *
 *                                                                            *
 ******************************************************************************/
int 
tuxctl_ioctl (struct tty_struct* tty, struct file* file, 
	      unsigned cmd, unsigned long arg)
{
	if (acknowledged == 0) return 0;			// overload protection
	int put_result;
    switch (cmd) {
	case TUX_INIT:		// turn on BIOC, set LED to user mode, turn off LEDs
		unsigned char cmd[4] = {MTCP_BIOC_ON, MTCP_LED_USR, MTCP_LED_SET, 0};
		put_result = tuxctl_ldisc_put(tty, cmd, 4);
		break;
	case TUX_BUTTONS:
		if (arg == 0) return -EINVAL;			// invalid ptr
		arg_store = (unsigned long*) arg;		// forced cast to ptr
		unsigned char cmd[1] = {MTCP_POLL};
		put_result = tuxctl_ldisc_put(tty, cmd, 1);
		break;
	case TUX_SET_LED:							// const len packet
		unsigned char cmd[6] = {MTCP_LED_SET, 0x0F, 0, 0, 0, 0};
		int led;
		for (led = 0; led < 4; led++) {
			if (arg & (0x00010000 << led))		// if LED is on
				cmd[led + 2] = LEDS[(arg >> (led * 4)) & 0x0F];
			if (arg & (0x01000000 << led))		// if dp is on
				cmd[led + 2] |= 0x10;
		}
		put_result = tuxctl_ldisc_put(tty, cmd, 6);
		break;
	default:
	    return -EINVAL;
    }
	if (put_result != 0) return -EINVAL;
	acknowledged = 0;							// "not responded yet"
	return 0;
}

