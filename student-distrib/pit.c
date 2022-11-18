#include "lib.h"
#include "types.h"
#include "pit.h"
#include "i8259.h"
// Add more if necessary
/* 
reference: https://wiki.osdev.org/PIT
*/

/* 
 * pic_init
 *  DESCRIPTION: Initialize pit timer chip
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: enable pic IRQ and set interrupt rate to 10ms
 */
void pit_init(void)
{
    /* set the flag
     * since cli_and_save needs long type, use 32 bits */
    // uint32_t flag;

    /* create the critical section */
	// cli_and_save(flag);     // disable interrupts

    // Set Mode/Command Register
    outb(READ_BACK_STATUS,MODE_REG);

    // Set interrupt period to 10ms
    uint16_t count = DEFAULT_RATE/TEN_MS;
    outb((count&HIGH_MASK),PIT_DATA_PORT);    // low bytes
    outb(((count&LOW_MASK)>>HIGH_SHIFT),PIT_DATA_PORT);   // high bytes

    // Connect to PIC
    enable_irq(PIT_IRQ);
    /* end the critical section */
	// restore_flags(flag);
	// sti();
}


/* 
 * pit_handler
 *  DESCRIPTION: handles interrupt and execute test_interrupt handler
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: execute schedule handler
 */
void pit_handler(void)
{
    // Send EOI
    send_eoi(PIT_IRQ);
    // Guarantee interrupts can happens
    // cli();
    // Change the next line to schedule handler later...
    //printf("PIT interrupt received!\n");
    scheduler();
    // sti();
}


