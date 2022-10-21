/* rtc.h - Defines used in interactions with Real Time Clock device
 */

#ifndef _RTC_H
#define _RTC_H

#include "types.h"


/* Ports that RTC sits on */
#define RTC_REG_PORT    0x70
#define RTC_DATA_PORT    0x71

/* RTC use irq 8 on MASTER_PIC */
#define RTC_IRQ_NUM 8

/* Select REG a,b,c with mask NMI */
#define REG_A 0x8A
#define REG_B 0x8B
#define REG_C 0x8C

/* About frequency */
#define ACTUAL_RATE 10
#define ACTUAL_FREQ 1024


/* Initialize RTC */
extern void rtc_init(void);
/* Set RTC frequency */
extern void rtc_set_freq(uint8_t );
/* RTC handler */
extern void rtc_handler(void);

#endif /* _I8259_H */
