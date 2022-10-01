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

    /*printk("cmd : %x %x %x\n", a, b, c); */
}

/******** IMPORTANT NOTE: READ THIS BEFORE IMPLEMENTING THE IOCTLS ************
 *                                                                            *
 * The ioctls should not spend any time waiting for responses to the commands *
 * they send to the controller. The data is sent over the serial line at      *
 * 9600 BAUD. At this rate, a byte takes approximately 1 millisecond to       *
 * transmit; this means that there will be about 9 milliseconds between       *
 * the time you request that the low-level serial driver send the             *
 * 6-byte SET_LEDS cmd and the time the 3-byte ACK cmd finishes         *
 * arriving. This is far too long a time for a system call to take. The       *
 * ioctls should return immediately with success if their parameters are      *
 * valid.                                                                     *
 *                                                                            *
 ******************************************************************************/
int 
tuxctl_ioctl (struct tty_struct* tty, struct file* file, 
	      unsigned cmd, unsigned long arg)
{
    switch (cmd) {
	case TUX_INIT:
		unsigned char cmd[1] = {MTCP_RESET_DEV};
		if (tuxctl_ldisc_put(tty, cmd, 1) != 0) return -EINVAL;
		unsigned char resp[3];
		tuxctl_ldisc_get(tty, resp, 3);
		if (resp[0] != MTCP_RESET) return -EINVAL;
		return 0;
	case TUX_BUTTONS:
		if (arg == 0) return -EINVAL;
		unsigned char cmd[1] = {MTCP_POLL};		// !! interrupt driven?
		if (tuxctl_ldisc_put(tty, cmd, 1) != 0) return -EINVAL;
		unsigned char resp[3];
		tuxctl_ldisc_get(tty, resp, 3);
		if (resp[0] != MTCP_POLL_OK) return -EINVAL;
		return (resp[1] & 0x0F) \				// _ _ _ _ C B A S
			| ((resp[2] & 0x09) << 4) \			// > _ _ ^ C B A S
			| ((resp[2] & 0x02) << 6) \			// > < _ ^ C B A S
			| ((resp[2] & 0x04) << 5);			// > < v ^ C B A S
	case TUX_SET_LED:
		unsigned char dps = (arg & 0x0F000000) >> 24;
		unsigned char flag = (arg & 0x000F0000) >> 16;
		unsigned char cmd[6] = {MTCP_LED_SET, flag, 0, 0, 0, 0};
		int led, packet_ptr = 2;				// Example: (LED2 in the 2nd position)
		for (led = 0; led < 4; led++) {			// cmd[3] = LEDS[(arg >> 8) & 0b1111]
			if (flag & (1 << led)) {			// cmd[3] |= (dps & 0b0100) << 2
				cmd[packet_ptr] = LEDS[(arg >> (led * 4)) & 0x0F];
				cmd[packet_ptr] |= (dps & (1 << led)) << (4 - led);
				packet_ptr++;
			}
		}
		if (tuxctl_ldisc_put(tty, cmd, packet_ptr) != 0) return -EINVAL;
		unsigned char resp[3];					// !! immediate return,
		tuxctl_ldisc_get(tty, resp, 3);			//    or wait for response?
		if (resp[0] != MTCP_LED_SET_OK) return -EINVAL;
		return 0;
	case TUX_LED_ACK:
	case TUX_LED_REQUEST:
	case TUX_READ_LED:
	default:
	    return -EINVAL;
    }
}

