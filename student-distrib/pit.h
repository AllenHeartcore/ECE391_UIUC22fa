#ifndef _PIT_H
#define _PIT_H

#define PIT_DATA_PORT       0x40
#define MODE_REG            0x43        // write only mode register
#define READ_BACK_STATUS    0x36        // low+high bytes, square wave generator
#define DEFAULT_RATE        1193181     // 1.193182 MHz
#define PIT_IRQ             0

void pit_init(void);
void pit_handler(void);

#endif
