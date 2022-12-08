#include "syscall.h"
#include "lib.h"
#include "types.h"
#include "x86_desc.h"
#include "filesys.h"
#include "rtc.h"
#include "idt_lnk.h"
#include "page.h"

/* Process id array */
uint32_t pid_array[MAX_PROCESS] = {0};

/* int32_t halt(uint8_t status)
 * Inputs: status - the status to return to the parent process
 * Outputs: none
 * Side Effects: none
 * Function: Halt the current process and return to the parent process
 */
int32_t halt(uint8_t status) {
    uint32_t ret_val, i;
    ret_val = (uint32_t)status;

    /* status = 0 means normal halt, status = 1 means exception */
    if (status == 255)
        /* According to document, return value 256 means exception calls halt. */
        ret_val = 256; 

    /* Check if the program about to halt is shell */
    pcb_t* cur_pcb = get_cur_pcb();
    uint32_t cur_pid = cur_pcb->cur_pid;
    if (cur_pcb->cur_pid == 0 || cur_pcb->cur_pid == 1 || cur_pcb->cur_pid == 2){

        /* Halt and re-open the base terminal.
         * - This allows the shell to be repeatedly re-opened
         *   after the previous instance has been closed.
         * - pid_array[0] is cleared so that the newly executed
         *   shell "inherits" pid #0, file descriptors, etc.
         * - From the user's perspective, the new shell appears
         *   identical to the old shell.
         */
        printf("WARNING: You cannot halt base shell.\n");
        pid_array[cur_pcb->cur_pid] = 0;           /* Close the current shell, */
        execute((uint8_t*)"shell"); /* and open a new shell */
    }

    /* Close all file descriptors */
    for (i = 0; i < MAX_OPENED_FILES; i++)
    {
        if (cur_pcb->file_descs[i].flags == 1) {
            cur_pcb->file_descs[i].file_operation.close_file(i);
            cur_pcb->file_descs[i].flags = 0;
        }
    }

    /* Deactivate current process */
    pid_array[cur_pid] = 0;
    for(i = 0; i < SCHEDULE_NUM; i++){
        if(schedule_array[i] == cur_pid)
            schedule_array[i] = cur_pcb->parent_pid;
    }


    /* Get parent pcb */
    pcb_t* parent_pcb = get_pcb(cur_pcb->parent_pid);

    /* Set paging for parent process */
	set_user_prog_page(cur_pcb->parent_pid);

    /* Prepare for the context switch */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MB - parent_pcb->cur_pid * EIGHT_KB - 4; /* Get the bottom of current process memory */
    /* We're expanding the expression instead of using the
     * "get_pcb(parent_pcb->cur_pid - 1)" function here, since
     * the meaning of "get_pcb(-1)" is ambiguous, and the call
     * would've failed had get_pcb checked argument validity. */

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

/* int32_t execute(const uint8_t* command)
 *
 * Inputs: command - the command to be executed
 * Outputs: -1 if fail, 0 if success
 * Side Effects: change `pid_array`
 * Function: Execute the issued command
 */
int32_t execute(const uint8_t* command) {
    int32_t i, j, program_start_addr, target_pid;
    uint8_t program_name[FILE_NAME_MAX] = {'\0'};
    int8_t args[KBD_BUF_SIZE + 1] = {'\0'};
    dentry_t temp_dentry;
    pcb_t* pcb;
    uint8_t elf_buff[4]; /* Testing ELF needs 4 bytes */
    if (command == NULL)
        return -1;

    /* Parse command */
    for (i = 0;
         i < strlen((int8_t*)command) &&
            command[i] != '\0' &&
            command[i] != ' ';
         i++) {
        program_name[i] = command[i];
    }

    /* Parse arguments */
    for (; i < strlen((int8_t*)command) &&
            command[i] != '\0' &&
            command[i] == ' ';
         i++);
    for (j = 0; i < strlen((int8_t*)command) &&
            command[i] != '\0';
         i++) {
        args[j++] = command[i];
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

    if(target_pid == 0 || target_pid == 1 || target_pid == 2){
        /* If target pid = 0,1,2, which means it is base shell, the parent is 255. */
        pcb->parent_pid = 255;
    }else{
        /* If target pid is not 0, assign parent pid as current pid. */
        pcb->parent_pid = get_cur_pid();
    }

    /* Set schedule array */
    for(i = 0; i < SCHEDULE_NUM; i++){
        if(schedule_array[i] == TERM_NOT_INIT || schedule_array[i] == pcb->parent_pid){
            schedule_array[i] = pcb->cur_pid;
            break;
        }
    }

    /* Open stdin stdout */
    pcb->file_descs[0].flags = 1;
    pcb->file_descs[0].inode = 0;
    pcb->file_descs[0].file_position = 0;
    pcb->file_descs[0].file_operation.open_file = illegal_open;
    pcb->file_descs[0].file_operation.close_file = illegal_close;
    pcb->file_descs[0].file_operation.read_file = terminal_read;
    pcb->file_descs[0].file_operation.write_file = illegal_write;

    pcb->file_descs[1].flags = 1;
    pcb->file_descs[1].inode = 0;
    pcb->file_descs[1].file_position = 0;
    pcb->file_descs[1].file_operation.open_file = illegal_open;
    pcb->file_descs[1].file_operation.close_file = illegal_close;
    pcb->file_descs[1].file_operation.read_file = illegal_read;
    pcb->file_descs[1].file_operation.write_file = terminal_write;

    /* Write arguments in pcb */
    memcpy(pcb->args, args, KBD_BUF_SIZE + 1);

    /* Terminal open calls are omitted.
     * According to the document, they should be illegal after all */

    /* Set up paging */
	set_user_prog_page(target_pid);

    /* Load program */
    read_data(temp_dentry.inode_num, 0, (uint8_t*)USER_CODE, USER_STACK - USER_CODE);

    /* Save old stack */
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    // TSS represents our destination
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (uint32_t)get_pcb(target_pid - 1) - 4;    // the child pid about to be created
    // PCB represents our source
    pcb->ex_ebp = saved_ebp;                 // so that we can return to the parent
    pcb->ex_esp = saved_esp;
    sti();
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

/*
 *   int32_t read(int32_t fd, void* buf, int32_t nbytes)
 *
 *   Reads n bytes to the buffer given. Returns the number of bytes read.
 *   INPUT: fd -- file descriptor
 *			buf -- buffer to read to
 *          nbytes -- number of bytes to read
 *   OUTPUT: number of bytes read, or -1 if fail
 *   SIDE EFFECT: Change the buffer
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes) {
	pcb_t	*current_pcb = get_cur_pcb();
    int32_t bytes_read;
	/* Read 0 byte if args are invalid */
	if (fd < 0 ||
		fd >= MAX_OPENED_FILES ||
		buf == NULL ||
		nbytes <= 0 ||
		!(current_pcb->file_descs[fd].flags)) {
		return -1;
	}

	bytes_read = current_pcb->file_descs[fd].file_operation.read_file(fd, buf, nbytes);
    current_pcb->file_descs[fd].file_position += bytes_read;
    return bytes_read;
}

/*
 *   write
 *   Writes n bytes from buffer given. Returns the number of bytes written.
 *   input: fd -- file descriptor
 *			buf -- buffer to write to
 *          nbytes -- number of bytes to write
 *   output: number of bytes written, or -1 failed
 *   side effect: Change the buffer
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
	pcb_t* current_pcb = get_cur_pcb();

	if (fd < 0 ||
		fd >= MAX_OPENED_FILES ||
		buf == NULL ||
		nbytes <= 0 ||
		!current_pcb->file_descs[fd].flags) {
		return -1;
	}

	return current_pcb->file_descs[fd].file_operation.write_file(fd, buf, nbytes);
}

/*
 *   open
 *   open a file in a filesystem and put file description in file descriptor array
 *   input: filename
 *   output: file descriptor for the opened file on success, -1 on fail
 *   side effect: Change the buffer
 */
int32_t open(const uint8_t* filename) {
    int i;
    int fd;
    dentry_t dentry;
    // check whether the filename is valid
    if (fopen(filename) == -1)
        return -1; 
    // get the current pcb
    pcb_t* cur_pcb = get_cur_pcb();
    // check whether number of opened files reach max
    for (i = 0; i < MAX_OPENED_FILES; i++){
        if (cur_pcb->file_descs[i].flags == 1)
            continue;
        else
            break;
    }
    if (i == MAX_OPENED_FILES)
        return -1;
    fd = i;
    read_dentry_by_name(filename, &dentry);
    cur_pcb->file_descs[fd].flags = 1;
    cur_pcb->file_descs[fd].inode = dentry.inode_num;
    cur_pcb->file_descs[fd].file_position = 0;
    // filetype = 1 is directory
    if (dentry.filetype == 1){
        cur_pcb->file_descs[fd].file_operation.open_file = dir_open;
        cur_pcb->file_descs[fd].file_operation.close_file = dir_close;
        cur_pcb->file_descs[fd].file_operation.read_file = dir_read;
        cur_pcb->file_descs[fd].file_operation.write_file = dir_write;
    }
    // 2 is the normal file
    else if (dentry.filetype == 2){
        cur_pcb->file_descs[fd].file_operation.open_file = fopen;
        cur_pcb->file_descs[fd].file_operation.close_file = fclose;
        cur_pcb->file_descs[fd].file_operation.read_file = fread;
        cur_pcb->file_descs[fd].file_operation.write_file = fwrite;
    }
    // 0 is the filetype of rtc
    else if (dentry.filetype == 0){
        cur_pcb->file_descs[fd].file_operation.open_file = rtc_open;
        cur_pcb->file_descs[fd].file_operation.close_file = rtc_close;
        cur_pcb->file_descs[fd].file_operation.read_file = rtc_read;
        cur_pcb->file_descs[fd].file_operation.write_file = rtc_write;
    }
    // abnormal filetype
    else{
        cur_pcb->file_descs[fd].flags = 0;
        return -1;
    }
    cur_pcb->file_descs[fd].file_operation.open_file(filename);
    return fd;
}

/*
 *   close
 *   close a file, remove the file from file descriptor array
 *   input: fd -- file descriptor
 *   output: 0 means success, -1 means fail
 *   side effect: Change the buffer
 */
int32_t close(int32_t fd) {
    // cannot delete file whose fd is 0 or 1 because they are stdin and stdout
    if (fd <= 1 || fd >= MAX_OPENED_FILES)
        return -1;
    pcb_t* pcb = get_cur_pcb();
    // check whether fd is valid
    if (!pcb->file_descs[fd].flags)
        return -1;
    pcb->file_descs[fd].file_operation.close_file(fd);
    pcb->file_descs[fd].flags = 0;
    return 0;
}

/*
 *   getargs
 *   copy the command line arguments into the buffer given by user
 *   input: fd -- file descriptor
 *          nbytes -- number of bytes to copy
 *   output: 0 means success, -1 means fail
 *   side effect: Change the buffer
 */
int32_t getargs(uint8_t* buf, int32_t nbytes) {
    pcb_t* current_pcb = get_cur_pcb();

    if (buf == NULL ||
        buf < (uint8_t*) USER_SPACE ||
        nbytes <= 0 ||
        current_pcb == NULL ||
        current_pcb->args[0] == '\0') {
        return -1;
    }

    memcpy(buf, current_pcb->args,
           nbytes > KBD_BUF_SIZE ? KBD_BUF_SIZE : nbytes);

    return 0;
}

/*
 *   vidmap
 *   map the text-mode video memory in kernel
 *   into user space at a given virtual address
 *   input: screen_start -- user-predefined virtual address
 *   output: 0 means success, -1 means fail
 *   side effect: Change the Page Directory and Page Table
 */
int32_t vidmap(uint8_t** screen_start) {
    if (screen_start == NULL ||
        screen_start < (uint8_t**) USER_SPACE ||
        screen_start > (uint8_t**) (USER_SPACE + FOUR_MB - 1)) {
        return -1;
    }
    set_vidmap_page(screen_start);
    return 0;
}

int32_t set_handler(int32_t signum, void* handler_address) {
    return 0;
}

int32_t sigreturn(void) {
    return 0;
}

/* ------------ Helper functions --------------- */

/*
 *   get_pcb
 *   get a pcb ptr with specific pid
 *   input: pid -- a pid
 *   output: a pointer to pcb
 *   side effect: none
 */
pcb_t* get_pcb(uint32_t pid) {
    /* (8MB - (pid+1) * 8KB) */
    return (pcb_t*)(EIGHT_MB - (pid+1) * EIGHT_KB);
}
/*
 *   get_cur_pid
 *   get current pid according to the stack's address
 *   input: none
 *   output: pid
 *   side effect: none
 */
uint32_t get_cur_pid(){
    register int32_t esp_val asm("esp");
    return ((EIGHT_MB-esp_val)/EIGHT_KB); 
}
/*
 *   get_cur_pcb
 *   get current pcb pointer according to the stack's address
 *   input: none
 *   output: a pointer to pcb
 *   side effect: none
 */
pcb_t* get_cur_pcb(){
    uint32_t cur_pid;
    cur_pid = get_cur_pid();
    return get_pcb(cur_pid);
}
