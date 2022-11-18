/* pit.h - constants and function interface for pit.c */
#ifndef _PIT_H
#define _PIT_H

#include "scheduler.h"
/* 
reference: https://wiki.osdev.org/PIT
*/
/* Constants */

// I/O Port
/* 
 * Channel0: connect to IRQ0
 * Channel1: originally refresh DRAM, now unusable
 * Channel2: connect to PC speaker
 * In our design, only Channel0 is used
*/
#define PIT_DATA_PORT 0x40   // Channel 0 data port (r/w)

// Mode Register
#define MODE_REG 0x43   // Mode/Command Register (write only)

// Read Back Status Byte
/*
; The Mode/Command register at I/O address 0x43 contains the following:
; Bit/s        Usage
; 7            Output pin state
; 6            Null count flags
; 4 and 5      Access mode :
;                 0 0 = Latch count value command
;                 0 1 = Access mode: lobyte only
;                 1 0 = Access mode: hibyte only
;                 1 1 = Access mode: lobyte/hibyte
; 1 to 3       Operating mode :
;                 0 0 0 = Mode 0 (interrupt on terminal count)
;                 0 0 1 = Mode 1 (hardware re-triggerable one-shot)
;                 0 1 0 = Mode 2 (rate generator)
;                 0 1 1 = Mode 3 (square wave generator)
;                 1 0 0 = Mode 4 (software triggered strobe)
;                 1 0 1 = Mode 5 (hardware triggered strobe)
;                 1 1 0 = Mode 2 (rate generator, same as 010b)
;                 1 1 1 = Mode 3 (square wave generator, same as 011b)
; 0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
*/
// We need Access mode: lobyte/hibyte and Operating mode 3 (square wave)
// i.e. 00110110 = 0x36
#define READ_BACK_STATUS 0x36

// Rate Settings
#define DEFAULT_RATE 1193181    //1.193182 MHz
#define TEN_MS  100 // 10 ms

// pit irq number
#define PIT_IRQ 0

// Avoid Magic Number
#define LOW_MASK    0xFF00
#define HIGH_MASK   0xFF
#define HIGH_SHIFT  8

/* Externally-visible functions */
/* Initialize pit timer chip */
void pit_init(void);
/* handles interrupt and execute test_interrupt handler */
void pit_handler(void);


#endif
