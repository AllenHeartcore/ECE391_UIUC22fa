#include "syscall.h"
#include "lib.h"
#include "scheduler.h"
#include "terminal.h"
#include "signal.h"
#include "lib.h"

int get_size_dosigreturn(){
    return DO_SIG_RETURN_END - DO_SIG_RETURN;
}


/*
 * kill_task
 * input: None
 * Output: None
 * default signal handler for signal number 0, 1 and 2
 */
void kill_task(){
    int i;
    pcb_t* cur_pcb = get_cur_pcb();
    for (i = 0; i < SIG_NUM; i++){
        cur_pcb->sig_mask[i] = SIG_UNMASK;
    }
    clear();
    halt(0);
    return ;
}
/*
 * ignore
 * input: None
 * Output: None
 * default signal handler for signal number 3 and 4
 */
void ignore(){
    int i;
    pcb_t* cur_pcb = get_cur_pcb();
    for (i = 0; i < SIG_NUM; i++){
        cur_pcb->sig_mask[i] = SIG_UNMASK;
    }
    return ;
}

/*
 * send_signal
 * input: signum
 * output: None
 * send a signal to the current program
 * side effect: None
 */

void send_signal(int sig_num){
    pcb_t* cur_pcb;
    cli();
    cur_pcb = get_cur_pcb();
    if (sig_num == SIG_INTERRUPT)
        cur_pcb = get_pcb(schedule_array[current_term_id]);
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
    uint32_t i, j;  
    uint32_t size;
    uint32_t sig_num = -1;  
    uint32_t* addr_signum = &sig_num;
    uint32_t return_addr;
    uint32_t* addr_return_addr;
    uint32_t user_esp;  
    void* handler;
    register uint32_t saved_ebp asm("ebp");
    hwcontext* context;
    cur_pcb = get_cur_pcb();
    for (i = 0; i < SIG_NUM; i++){
        if (cur_pcb->signal[i] == SIG_ACTIVATE){
            cur_pcb->signal[i] = SIG_DEACTIVATE;
            for (j = 0; j < SIG_NUM; j++)
                cur_pcb->sig_mask[j] = SIG_MASK;
            sig_num = i;
            break;
        }
    }
    if (sig_num == -1)
        return ;
    // get the h/w context address
    // --------
    // old ebp
    // return address
    // h/w context
    // --------
    handler = cur_pcb->signal_handler[sig_num];
    if (handler == kill_task || handler == ignore){
        ((void(*)())handler)();
        return;
    }
    context = (hwcontext*)(saved_ebp + 8);
    user_esp = context->ESP;
    // if interrupted in the kernel space, then don't handle the signal
    if (user_esp < USER_SPACE)
        return ;
    size = get_size_dosigreturn();
    // printf(context->return_address);
    return_addr = user_esp - size;
    addr_return_addr = &return_addr;
    // push the execute sigreturn code 
    memcpy((void*)(user_esp - size), DO_SIG_RETURN, size);
    // push the h/w context
    memcpy((void*)(user_esp - size - sizeof(hwcontext)), context, sizeof(hwcontext));
    // push the signal number
    memcpy((void*)(user_esp - size - sizeof(hwcontext) - 4), addr_signum, 4);
    // push the return address
    memcpy((void*)(user_esp - size - sizeof(hwcontext) - 8), (void*)addr_return_addr, 4);
    // set the new esp in h/w context for iret
    context->return_address = (uint32_t)handler;
    user_esp = user_esp - size - sizeof(hwcontext) - 8;
    context->ESP = user_esp;
    return ;
}

