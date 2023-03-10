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

static bool acknowledged = 1;								// overload protection
static unsigned char buttons;
static spinlock_t lock;										// for concurrent R/W
static int irqsaved;
static unsigned char LEDs[4];
const unsigned char LED_lookup[16] = {0xE7, 0x06, 0xCB, 0x8F, 0x2E, 0xAD, 0xED, 0xA6,\
									  0xEF, 0xAF, 0xEE, 0x6D, 0xE1, 0x4F, 0xE9, 0xE8};

/************************ Protocol Implementation *************************/
// @@ CHECKPOINT 2: Tux "receiver"

void tuxctl_handle_ack (void) {
	acknowledged = 1;
}

void tuxctl_handle_bioc_event (unsigned resp1, unsigned resp2) {
	if (resp1 != 0x80 || resp2 != 0x80) {					// valid if not "80 80"
		spin_lock_irqsave(&lock, irqsaved);
		buttons = resp1 & 0x0F;								// _ _ _ _ C B A S
		buttons |= (resp2 & 0x09) << 4;						// > _ _ ^ C B A S
		buttons |= (resp2 & 0x02) << 5;						// > < _ ^ C B A S
		buttons |= (resp2 & 0x04) << 3;						// > < v ^ C B A S
		spin_unlock_irqrestore(&lock, irqsaved);
	}
}

void tuxctl_handle_reset (struct tty_struct* tty) {
	unsigned char cmd[8] = {MTCP_BIOC_ON, MTCP_LED_USR, MTCP_LED_SET, 0x0F, 0, 0, 0, 0};
	int LED;
	for (LED = 0; LED < 4; LED++) cmd[LED + 4] = LEDs[LED];	// restore
	if (!acknowledged) return;
	tuxctl_ldisc_put(tty, cmd, 8);
	acknowledged = 0;
}

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
	switch (a) {											// some kind of dispatcher
	case MTCP_ACK:			tuxctl_handle_ack();
	case MTCP_BIOC_EVENT:	tuxctl_handle_bioc_event(b, c);
	case MTCP_RESET:		tuxctl_handle_reset(tty);	
	default:				return;
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
// @@ CHECKPOINT 2: Tux "sender"

int tuxctl_ioctl_init (struct tty_struct* tty) {
	unsigned char cmd[4] = {MTCP_BIOC_ON, MTCP_LED_USR, MTCP_LED_SET, 0};
	if (!acknowledged) return 0;							// turn on BIOC + ...
	tuxctl_ldisc_put(tty, cmd, 4);							// set LEDs to user mode + ...
	acknowledged = 0;										// turn off LEDs
	buttons = 0xFF;
	lock = SPIN_LOCK_UNLOCKED;
	return 0;
}

int tuxctl_ioctl_buttons (unsigned long* ptr) {
	if (!ptr) return -EINVAL;								// invalid ptr
	spin_lock_irqsave(&lock, irqsaved);
	*ptr = (unsigned long) buttons;							// copy to user
	spin_unlock_irqrestore(&lock, irqsaved);
	return 0;
}

int tuxctl_ioctl_set_led (struct tty_struct* tty, unsigned long arg) {
	unsigned char cmd[6] = {MTCP_LED_SET, 0x0F, 0, 0, 0, 0};
	int LED;												// ^ const len packet
	for (LED = 0; LED < 4; LED++) {
		if (arg & (0x00010000 << LED))						// if LED is on
			cmd[LED + 2] = LED_lookup[(arg >> (LED * 4)) & 0x0F];
		if (arg & (0x01000000 << LED))						// if dp is on
			cmd[LED + 2] |= 0x10;
		LEDs[LED] = cmd[LED + 2];
	}
	if (!acknowledged) return 0;							// ignore, instead of...
	tuxctl_ldisc_put(tty, cmd, 6);							// returning -EINVAL, if...
	acknowledged = 0;										// overload, since it's...
	return 0;												// not the user's fault
}

int tuxctl_ioctl (struct tty_struct* tty, struct file* file, 
	      unsigned cmd, unsigned long arg) {
	switch (cmd) {
	case TUX_INIT:			return tuxctl_ioctl_init(tty);
	case TUX_BUTTONS:		return tuxctl_ioctl_buttons((unsigned long*) arg);
	case TUX_SET_LED:		return tuxctl_ioctl_set_led(tty, arg);
	default:				return -EINVAL;
    }
}
