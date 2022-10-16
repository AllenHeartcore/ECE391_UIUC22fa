/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    /* Mask all the interrupts */
    outb(0xFF,MASTER_8259_PORT+1);
    outb(0xFF,SLAVE_8259_PORT+1);

    /* Initialize Mastr PIC */
    outb(ICW1,MASTER_8259_PORT);
    outb(ICW2_MASTER,MASTER_8259_PORT+1);
    outb(ICW3_MASTER,MASTER_8259_PORT+1);
    outb(ICW4,MASTER_8259_PORT+1);

    /* Initialize Slave PIC */
    outb(ICW1,SLAVE_8259_PORT);
    outb(ICW2_SLAVE,SLAVE_8259_PORT+1);
    outb(ICW3_SLAVE,SLAVE_8259_PORT+1);
    outb(ICW4,SLAVE_8259_PORT+1);

    /* Initialize state variable indicating mask register in PICs */
    master_mask = 0xFF;
    slave_mask = 0xFF;
    enable_irq(2); // Enable the slave PIC. Slave PIC is on 2 port on Master PIC

}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    /* Sanity check, if irq_num>15, return */
    if(irq_num>15){
        return;
    }
    if(irq_num>7){
        /* The irq is on the slave PIC */
        slave_mask = slave_mask & ~(1<<(irq_num-8));
        outb(slave_mask,SLAVE_8259_PORT+1);
    }else{
        /* The irq is on the master PIC */
        master_mask = master_mask & ~(1<<irq_num);
        outb(master_mask,MASTER_8259_PORT+1);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    /* Sanity check, if irq_num>15, return */
    if(irq_num>15){
        return;
    }
    if(irq_num>7){
        /* The irq is on the slave PIC */
        slave_mask = slave_mask | (1<<(irq_num-8));
        outb(slave_mask,SLAVE_8259_PORT);
    }else{
        /* The irq is on the master PIC */
        master_mask = master_mask | (1<<irq_num);
        outb(master_mask,MASTER_8259_PORT);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    /* Sanity check */
    if (irq_num>15)
        return;
    if(irq_num>=8){
        /* For slave PIC, both slave and master pic should receive EOI */
        outb(EOI | (irq_num-8), SLAVE_8259_PORT+1); // Send to SLAVE PIC
        outb(EOI | 2, MASTER_8259_PORT+1); // Send to MASTER PIC telling SLAVE PIC's hanlder has finished
        return;
    }
    /* Send MASTER Pic */
    outb(EOI | irq_num, MASTER_8259_PORT+1);
}
