#include "syscall.h"
#include "stdio.h"
#include "scheduler.h"
#include "terminal.h"
#include "signal.h"

int default_signal_handler(){
    return 0;
}

void send_signal(int sig_num){
    pcb_t* cur_pcb;
    cli();
    cur_pcb = get_cur_pcb();
    if (sig_num = SIG_INTERRUPT)
        cur_pcb = get_pcb(schedule_array[current_term_id]);
    if (cur_pcb->sig_mask[sig_num] == SIG_MASK){
        sti();
        return ;
    }
    cur_pcb->signal[sig_num] = SIG_ACTIVATE;
    sti();
}

/*
 * handle_signal
 * input: None
 * output: None
 * Check whether there is pending signal in current process, 
 * if so, build the stack in user stack
 * if not, do nothing
 */
void handle_signal(){
    pcb_t* cur_pcb;
    // loop iterator
    int i;  
    int sig_num = -1;    
    register uint32_t saved_ebp asm("ebp");
    hwcontext* context;
    context = saved_ebp + 8;
    printf(context->return_address);
    cur_pcb = get_cur_pcb();
    for (i = 0; i < SIG_NUM; i++){
        if (cur_pcb->signal[i] == SIG_ACTIVATE){
            sig_num = i;
            break;
        }
    }
    if (sig_num == -1)
        return ;
    asm volatile ()
}

#define DO_SIG_RETURN() do{
    asm volatile (
        "
        DO_SIG_RETURN:              \n\
            movl    $10, %%eax      \n\
            int     0x80            \n\
            RET                     \n\
        DO_SIG_RETURN_END:          \n\
        "
        : 
        : 
        : "eax"
    )
}while (0)