/* rtc.c - Functions to interact with RTC
 * Updated in checkpoint 2: The RTC is now virtualized
 */

#include "rtc.h"
#include "lib.h"
#include "i8259.h"

volatile uint32_t rtc_time_tic; // (aka. "ms") Each time RTC interrupt happens, it increases 1.
volatile uint32_t rtc_sec;      // (aka. "s") How many seconds since the RTC has been initialized.
volatile bool rtc_int_occurred; // Flag to indicate if a **virtual** RTC interrupt has occurred.
uint32_t freq;                  // This is VIRTUALIZED (actual freq is always 1024 Hz)

/*  More on virtual interrupts:
 *  The `rtc_int_occurred` flag is to be used by `rtc_open`
 *  to determine when it should return. It is managed by
 *  `rtc_handler` and is only set when a **virtual** interrupt
 *  occurs (at the rate of `freq`). Actual interrupts occur
 *  at ACTUAL_FREQ, so we only set the flag if `rtc_time_tic`
 *  (or "actual interrupt count") can be divided by (ACTUAL_FREQ / freq).
 *  In effect, the flag is set at the virtual rate.
 */

/*
*   rtc_init
*   Initialize RTC
*   input: int rate
*   output: None
*   side effect: The RTC device will be initialized and PIC's 8th irq will be enabled.
*/
void rtc_init(void) {
    char prev;
    outb(REG_B,RTC_REG_PORT);
    prev = inb(RTC_DATA_PORT);  // Read current value from RegB
    outb(REG_B,RTC_REG_PORT);
    outb(prev|0x40,RTC_DATA_PORT); // Turns on the 6 bit in RegB

    outb(REG_A,RTC_REG_PORT);   // Frequency of virtualized RTC...
    prev = inb(RTC_DATA_PORT);  // is fixed to 1024 Hz (rate = 10)
    outb(REG_A,RTC_REG_PORT);

    /* Only change the bits related to rate in RegA(low 4 bits)*/
    outb((prev & 0xF0)|ACTUAL_RATE, RTC_DATA_PORT); 

    /* Change the state variable freq */
    freq = 2;                   // the default VIRTUAL value (same as "rtc_open")

    rtc_time_tic = 0;
    rtc_sec = 0;
    enable_irq(RTC_IRQ_NUM);
}

/*  rtc_open
 *  Reset the RTC to default frequency (2 Hz)
 *  input: (Ignored)
 *  output: 0
 *  side effect: "freq" will be reset to default (2 Hz)
 */
int rtc_open(const uint8_t* filename) {
	freq = 2;
	return 0;
}

/*  rtc_close
 *  Essencially does nothing...for now. An array to track freqs 
 *  for multiple processes will be implemented in later checkpoints,
 *  and this function will be used to clear all stored values.
 *  input: (Ignored)
 *  output: 0
 *  side effect: None
 */
int rtc_close(int32_t fd) {
	return 0;
}

/*
 *  rtc_read
 *  Read the RTC's built-in timer (returns only when an RTC interrupt has occurred)
 *  input: (Ignored)
 *  output: 0 at the next virtual interrupt
 *  side effect: None
 */
int rtc_read(int32_t fd, void* buf, int32_t nbytes) {
	rtc_int_occurred = 0;
	while (!rtc_int_occurred);	// return at the virtual interrupt
	return 0;
}

/*
 *  rtc_write
 *  Set the RTC's frequency
 *  input: buffer containing the requested frequency; others are ignored
 *  output: 0 if success, -1 if fail
 *  side effect: The RTC's frequency will be set as requested (if it's a power of 2).
 */
int rtc_write(int32_t fd, const void* buf, int32_t nbytes) {
    char prev;
	uint32_t freq_requested = *(uint32_t*) buf;
    /* Sanity check */
	if (freq_requested <= 0 || freq_requested > ACTUAL_FREQ || \
		(freq_requested & (freq_requested - 1)))
		return -1;	// fail if "out of range" or "not power of 2"
	freq = freq_requested;
	return 0;
}

/*
*   rtc_handler
*   Handler function for RTC
*   input: None
*   output: None
*   side effect: The state variable sec and tic will be changed.
*/
void rtc_handler(void) {
    cli();
    char temp;
    rtc_time_tic++;
    if (rtc_time_tic % ACTUAL_FREQ) rtc_sec++;	// one second has passed
	if (rtc_time_tic % (ACTUAL_FREQ / freq))
		rtc_int_occurred = 1;	// delayed virtual interrupt
    /* To make sure rtc can raise intterrupt later */
    outb(REG_C&0xF,RTC_REG_PORT);
    temp = inb(RTC_DATA_PORT);
    send_eoi(RTC_IRQ_NUM);
    sti();
}

