#ifdef _USERSPACE

/* Alias the ioctl and open system calls to our own */
#define ioctl __mp1_ioctl
#define open __mp1_open

/* Stub out the kernel calls since we're in userspace */
#define mp1_copy_to_user __mp1_copy_to_user
#define mp1_copy_from_user __mp1_copy_from_user
#define __user 

#endif

#ifndef _ASM
#include <linux/rtc.h>
#include <linux/ioctl.h>
#include "mp1_userspace.h"

#define RTC_STARTGAME 	_IO('M', 0x13) 
#define RTC_ADDMISSILE 	_IO('M', 0x14)  
#define RTC_MOVEXHAIRS	_IO('M', 0x15)
#define RTC_GETSTATUS 	_IO('M', 0x16)
#define RTC_ENDGAME 	_IO('M', 0x17)

struct missile {
    struct missile* next;   /* pointer to next missile in linked list */
    int x, y;		    /* x,y position on screen                 */
    int vx, vy;		    /* x,y velocity vector                    */
    int dest_x, dest_y;     /* location at which the missile explodes */
    int exploded;           /* explosion duration counter             */
    char c;                 /* character to draw for this missile     */
};

extern void mp1_free(void*);
extern void* mp1_malloc(int);
extern void mp1_notify_user(void);

#endif
