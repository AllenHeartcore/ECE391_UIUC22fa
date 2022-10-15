/* rtc.c - Functions to interact with RTC
 */

#include "rtc.h"
#include "lib.h"


volatile uint32_t rtc_time_tic; // Time tic  Each time RTC interrupt happens, it increases 1.
uint32_t rtc_sec;  // How many seconds since the RTC has been initialized.


/* Initialize RTC */
void rtc_init(void) {
    char prev;
    cli();
    outb(REG_B,RTC_REG_PORT);
    prev = inb(RTC_DATA_PORT);  // Read current value from RegB
    outb(REG_B,RTC_REG_PORT);
    outb(prev|0x40,RTC_DATA_PORT); // Turns on the 6 bit in RegB
    rtc_set_freq(6);
    sti();
    enable_irq(RTC_IRQ_NUM);
}


/* Set RTC's frequency 
*/
void rtc_set_freq(uint8_t rate){

}

/* Handler function for RTC */
void rtc_handler(void) {
    rtc_time_tic++;
}

