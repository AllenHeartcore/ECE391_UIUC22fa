#include "lib.h"
#include "types.h"
#include "pit.h"
#include "i8259.h"
#include "scheduler.h"

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
    /* Set Mode/Command Register */
    outb(READ_BACK_STATUS, MODE_REG);

    /* Set interrupt period to 10ms */
    uint16_t period = DEFAULT_RATE / 100;
    outb((period & 0xFF), PIT_DATA_PORT);
    outb(((period & 0xFF00) >> 8), PIT_DATA_PORT);

    /* Enable IRQ0 */
    enable_irq(PIT_IRQ);
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
    send_eoi(PIT_IRQ);
    scheduler();
}
