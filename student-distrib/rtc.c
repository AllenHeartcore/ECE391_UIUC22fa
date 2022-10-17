/* rtc.c - Functions to interact with RTC
 */

#include "rtc.h"
#include "lib.h"
#include "i8259.h"

volatile uint32_t rtc_time_tic; // Time tic  Each time RTC interrupt happens, it increases 1.
volatile uint32_t rtc_sec;  // How many seconds since the RTC has been initialized.
uint32_t freq;

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
    rtc_set_freq(6);
    rtc_time_tic = 0;
    rtc_sec = 0;
    enable_irq(RTC_IRQ_NUM);
}



/*
*   rtc_set_freq
*   Set RTC's frequency  frequency =  2^15>>(rate-1)
*   input: int rate
*   output: None
*   side effect: The RTC device's frequency will be changed. 
*/
void rtc_set_freq(uint8_t rate){
    char prev;
    /* Sanity check */
    if(rate<MIN_RATE || rate>MAX_RATE){
        return;
    }
    outb(REG_A,RTC_REG_PORT);
    prev = inb(RTC_DATA_PORT);
    outb(REG_A,RTC_REG_PORT);
    
    /* Only change the bits related to rate in RegA(low 4 bits)*/
    outb((prev & 0xF0)|rate, RTC_DATA_PORT); 

    /* Change the state variable freq */
    freq = 32768>>(rate-1); // 2^15>>(rate-1)
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
    if(rtc_time_tic==freq){
        /* If one second has passed, reset timetic and plus one to sec */
        rtc_sec++;
        rtc_time_tic=0;
        // printf("In RTC!!\n");
    }
    /* To make sure rtc can raise intterrupt later */
    outb(REG_C&0xF,RTC_REG_PORT);
    temp = inb(RTC_DATA_PORT);
    send_eoi(RTC_IRQ_NUM);
    sti();
}

