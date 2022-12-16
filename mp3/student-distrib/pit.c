#include "lib.h"
#include "types.h"
#include "pit.h"
#include "scheduler.h"
#include "i8259.h"
#include "signal.h"

static int counter = 0;
/*
 * pit_init
 *  DESCRIPTION: initialize the pit 
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: preempt the IRQ0
 */
void pit_init(void)
{
    outb(PIT_MODE, PIT_MODE_PORT);
    outb(PIT_FREQ & LOW_EIGHT_BIT, PIT_DATA_PORT);
    // right shift 8 to get high eight bit of freq
    outb((PIT_FREQ & HIGH_EIGHT_BIT) >> 8, PIT_DATA_PORT);
    enable_irq(PIT_IRQ);
    return ;
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
    // end the interrupt to let the next interrupt work
    counter++;
    // evert 10 sec send a signal
    if (counter == 1000){
        send_signal(SIG_ALARM);
        counter = 0;
    }
    send_eoi(PIT_IRQ);
    scheduler();
}
