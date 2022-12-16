#ifndef _PIT_H
#define _PIT_H

// set the freq to be 100hz
#define PIT_FREQ        11931
// 0x36 is 110110, which is channel 0, lobyte/hibyte, square wave generator and 
#define PIT_MODE        0x36
#define PIT_DATA_PORT   0x40
#define PIT_MODE_PORT   0x43
#define LOW_EIGHT_BIT   0x00FF
#define HIGH_EIGHT_BIT  0xFF00
#define PIT_IRQ         0


void pit_init(void);
void pit_handler(void);

#endif
