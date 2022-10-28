#include "syscall.h"
#include "lib.h"
#include "types.h"

int32_t halt(uint8_t status) {
    return 0;
}

int32_t execute(const uint8_t* command) {
    int32_t i;
    uint8_t program_name[FILE_NAME_MAX];
    /* Parse command */
    for(i<0;i++;i<len(command)){
        if(command[i]==' ')
            break;
    }
    /* Check file validity */

    /* Create PCB */

    /* Set up paging */

    /* Load program */

    /* Save old stack & prepare iret params */

    /* Enter user mode */

    return 0;
}

/*
 *   read
 *   Reads a function to the buffer given. Returns the number of bytes read.
 *   input: fd -- file descriptor
 *			buf -- buffer to read to
 *          nbytes -- number of bytes to read
 *   output: None
 *   side effect: Change the buffer
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes) {
	pcb_t	*current_pcb = get_cur_pcb();

	/* Read 0 byte if args are invalid */
	if (fd < 0 ||
		fd >= MAX_OPENED_FILES ||
		buf == NULL ||
		nbytes < 0 ||
		!(current_pcb->file_descs[fd].flags.valid_desc)) {
		return 0;
	}

	return current_pcb->file_descs[fd].file_operation->read_file(fd, buf, nbytes);
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    return 0;
}

int32_t open(const uint8_t* filename) {
    if (fopen(filename) == 0)
        return 0; 
    
    return -1;
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

