#include "syscall.h"
#include "lib.h"
#include "types.h"
#include "x86_desc.h"

int32_t halt(uint8_t status) {
    /* status = 0 means normal halt, status = 1 means exception */



    return 0;
}

int32_t execute(const uint8_t* command) {

    /* Parse command */

    /* Check file validity */

    /* Create PCB */

    /* Set up paging */

    /* Load program */

    /* Save old stack */
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    // TSS represents our destination
    tss.ss0 = KERNEL_DS;
    tss.esp0 = get_pcb(target_pid - 1) - 4;    // the child pid about to be created
    // PCB represents our source
    pcb.ex_ebp = saved_ebp;                 // so that we can return to the parent
    pcb.ex_esp = saved_esp;

    /* Enter user mode */
    // Push order: SS, ESP, EFLAGS, CS, EIP
    // ESP points to the base of user stack (132MB - 4B)
    asm volatile("pushl %0 \n\
                  pushl %1 \n\
                  pushfl   \n\
                  pushl %2 \n\
                  pushl %3 \n\
                  iret     \n"
                : /* no output */
                : "r" (USER_DS), \
                  "r" (0x8400000 - sizeof(uint32_t)), \
                  "r" (USER_CS), \
                  "r" (program_start_addr)
                : "memory");

    return 0;
}

int32_t read(int32_t fd, void* buf, int32_t nbytes) {
    return 0;
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    return 0;
}

int32_t open(const uint8_t* filename) {
    return 0;
}

int32_t close(int32_t fd) {
    return 0;
}

int32_t getargs(uint8_t* buf, int32_t nbytes) {
    return 0;
}

int32_t vidmap(uint8_t** screen_start) {
    return 0;
}

int32_t set_handler(int32_t signum, void* handler_address) {
    return 0;
}

int32_t sigreturn(void) {
    return 0;
}
