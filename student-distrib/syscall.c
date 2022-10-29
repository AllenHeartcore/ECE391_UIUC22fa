#include "syscall.h"
#include "lib.h"
#include "types.h"
#include "x86_desc.h"
#include "filesys.h"
#include "idt_lnk.h"
#include "page.h"
#include "terminal.h"

#define MAX_PROCESS 6          // At most 6 processes and...
#define MAX_OPENED_FILES 8     // 8 files can be concurrently opened
#define EIGHT_MB    0x800000   // 8 MB
#define EIGHT_KB    0x2000     // 8 KB
#define USER_SPACE  0x8000000  // 128 MB
#define USER_STACK  0x8400000  // 132 MB

/* Process id array */
uint32_t pid_array[MAX_PROCESS] = {0};



int32_t halt(uint8_t status) {
    uint32_t ret_val;
    ret_val = 0;

    /* status = 0 means normal halt, status = 1 means exception */
    if (status == 1)
        /* According to document, return value 256 means exception calls halt. */
        ret_val = 256; 

    /* Check if the program about to halt is shell */
    pcb_t* cur_pcb = get_cur_pcb();
    uint32_t cur_pid = cur_pcb->cur_pid;
    if (cur_pcb->cur_pid == 0){
        /* TODO: Restart shell */
    }

    /* Close all file descriptors */
    for (int i = 0; i < MAX_OPENED_FILES; i++)
    {
        /* TODO: Set all descriptors to 0 */
        cur_pcb->file_descs;
    }

    /* Deactivate current process */
    pid_array[cur_pid] = 0;

    /* Get parent pcb */
    pcb_t* parent_pcb = get_pcb(cur_pcb->parent_pid);

    /* Set paging for parent process */

    /* Prepare for the context switch */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (uint32_t)get_pcb(parent_pcb->cur_pid - 1) - 4; /* Get the bottom of current process memory */

    asm volatile("movl %0, %%eax \n\
                  movl %1, %%ebp \n\
                  movl %2, %%esp \n\
                  leave          \n\
                  ret            \n"
                : /* no output */
                : "r" (ret_val), \
                  "r" (cur_pcb->ex_ebp), \
                  "r" (cur_pcb->ex_esp)
                : "eax", "ebp", "esp");
    return 0;
}

int32_t execute(const uint8_t* command) {
    int32_t i, program_start_addr, target_pid;
    uint8_t program_name[FILE_NAME_MAX] = {'\0'};
    dentry_t temp_dentry;
    pcb_t* pcb;
    uint8_t elf_buff[4]; /* Testing ELF needs 4 bytes */

    if (command == NULL)
        return -1;

    /* Parse command */
    for (i = 0; i < strlen((int8_t*)command); i++) {
        if(command[i] == ' ')
            break;
        program_name[i] = command[i];
    }

    /* Check file validity */
    if (read_dentry_by_name(program_name, &temp_dentry) == -1)
        return -1;

    /* Check if we can read front 4 bytes from the file */
    if(4 != read_data(temp_dentry.inode_num, 0, elf_buff, 4))
        return -1;

    /* Check if it is executeable: ELF's magic number = 0x7f454c46 */
    if (elf_buff[0] != 0x7f || elf_buff[1] != 0x45 || \
        elf_buff[2] != 0x4c || elf_buff[3] != 0x46)
        return -1;

    /* Get the program start addr  According to document, the start address is in 24-27 bytes */
    read_data(temp_dentry.inode_num, 24, (uint8_t*)(&program_start_addr), 4);

    /* Create PCB */
    for (i = 0; i < MAX_PROCESS; i++) {   // get a pid
        if (pid_array[i] == 0)
            break;
    }
    if (i == MAX_PROCESS)                 // no pid can be used
        return -1;
    pid_array[i] = 1;           // activate pid
    target_pid = i;
    /* Get pcb structure */
    pcb = get_pcb(target_pid);
    pcb->cur_pid = target_pid;

    if(target_pid == 0){
        /* If target pid = 0, which means it is shell, the parent is 0 itself. */
        pcb->parent_pid = 0;
    }else{
        /* If target pid is not 0, assign parent pid as current pid. */
        pcb->parent_pid = get_cur_pid();
    }

    /* Open stdin stdout */
    pcb->file_descs[0].flags = 1;
    pcb->file_descs[0].file_operation->open_file = terminal_open;
    pcb->file_descs[0].file_operation->close_file = terminal_close;
    pcb->file_descs[0].file_operation->read_file = stdin_read;
    pcb->file_descs[0].file_operation->write_file = terminal_write;

    pcb->file_descs[1].flags = 1;
    pcb->file_descs[1].file_operation->open_file = terminal_open;
    pcb->file_descs[1].file_operation->close_file = terminal_close;
    pcb->file_descs[1].file_operation->read_file = terminal_read;
    pcb->file_descs[1].file_operation->write_file = stdout_write;

    /* Set up paging */

    /* Load program */

    /* Save old stack */
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    // TSS represents our destination
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (uint32_t)get_pcb(target_pid - 1) - 4;    // the child pid about to be created
    // PCB represents our source
    pcb->ex_ebp = saved_ebp;                 // so that we can return to the parent
    pcb->ex_esp = saved_esp;

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
                  "r" (USER_STACK - sizeof(uint32_t)), \
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

/* ------------ Helper functions --------------- */

pcb_t* get_pcb(uint32_t pid) {
    /* (8MB - (pid+1) * 8KB) */
    return (pcb_t*)(EIGHT_MB - (pid+1) * EIGHT_KB);
}

uint32_t get_cur_pid(){
    register int32_t esp_val asm("esp");
    return ((EIGHT_MB-esp_val)/EIGHT_KB); 
}

pcb_t* get_cur_pcb(){
    uint32_t cur_pid;
    cur_pid = get_cur_pid();
    return get_pcb(cur_pid);
}


