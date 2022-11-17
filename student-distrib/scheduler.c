#include "scheduler.h"
#include "syscall.h"
#include "page.h"
#include "x86_desc.h"
#include "lib.h"
int32_t schedule_array [SCHEDULE_NUM] = {TERM_NOT_INIT,TERM_NOT_INIT,TERM_NOT_INIT};
int8_t  cur_sch_index=0; // current scheduled index
void scheduler(){

    /* save ebp */
    pcb_t* current_pcb = get_cur_pcb();
    register uint32_t saved_ebp asm("ebp");
    current_pcb->sch_ebp = saved_ebp;

    /* Check next scheduled process */
    cur_sch_index =(cur_sch_index + 1)%SCHEDULE_NUM;
    int32_t next_pid = schedule_array[(int32_t)cur_sch_index];
    /* If next terminal has not been initialized, execute shell */
    if(next_pid==TERM_NOT_INIT){
        /* For a new terminal, it should write to backup buffer */
        remap_vidmap_page(cur_sch_index);
        execute((uint8_t*)"shell");
    }


    /* Remap the user program */
    set_user_prog_page(next_pid);
    
    /* Check backup buffer and video memory */
    remap_vidmap_page(cur_sch_index);

    /* Set tss */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (uint32_t)get_pcb(next_pid - 1) - 4; 

    pcb_t* next_pcb = get_pcb(next_pid);
    asm volatile(
               "movl %0, %%ebp \n\
                leave          \n\
                ret            \n"
            :  /* no output */
            :  "r" (next_pcb ->sch_ebp) \
            :  "ebp");
}
