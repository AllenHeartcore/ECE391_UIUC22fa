/* rtc.c - Functions to interact with RTC
 */

#include "rtc.h"
#include "lib.h"


volatile uint32_t rtc_time_tic; // Time tic  Each time RTC interrupt happens, it increases 1.
uint32_t rtc_sec;  // How many seconds since the RTC has been initialized.


/* Initialize RTC */
void rtc_init(void) {


}


/* Set RTC's frequency */
void rtc_set_freq(uint32_t frequency){

}

/* Handler function for RTC */
void rtc_handler(void) {
}

