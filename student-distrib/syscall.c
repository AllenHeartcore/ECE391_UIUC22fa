#include "syscall.h"
#include "lib.h"
#include "types.h"
#include "rtc.h"

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
 *   int32_t read(int32_t fd, void* buf, int32_t nbytes)
 *
 *   Reads n bytes to the buffer given. Returns the number of bytes read.
 *   INPUT: fd -- file descriptor
 *			buf -- buffer to read to
 *          nbytes -- number of bytes to read
 *   OUTPUT: None
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
		return 0;
	}

	bytes_read = current_pcb->file_descs[fd].file_operation->read_file(fd, buf, nbytes);
    current_pcb->file_descs[fd].file_position += bytes_read;
    return bytes_read;
}

/*
 *   write
 *   Reads n bytes from buffer given. Returns the number of bytes written.
 *   input: fd -- file descriptor
 *			buf -- buffer to write to
 *          nbytes -- number of bytes to read
 *   output: None
 *   side effect: Change the buffer
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
	pcb_t* current_pcb = get_cur_pcb();

	if (fd < 0 ||
		fd >= MAX_OPENED_FILES ||
		buf == NULL ||
		nbytes <= 0 ||
		!current_pcb->file_descs[fd].flags) {
		return 0;
	}

	return current_pcb->file_descs[fd].file_operation->write_file(fd, buf, nbytes);
}

int32_t open(const uint8_t* filename) {
    int i;
    int fd;
    dentry_t dentry;
    if (fopen(filename) == -1)
        return -1; 
    pcb_t* cur_pcb = get_cur_pcb();
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
    // 1 and 2is the filetype of d
    if (dentry.filetype == 1 || dentry.filetype == 2){
        cur_pcb->file_descs[fd].file_operation->open_file = fopen;
        cur_pcb->file_descs[fd].file_operation->close_file = fclose;
        cur_pcb->file_descs[fd].file_operation->read_file = fread;
        cur_pcb->file_descs[fd].file_operation->write_file = fwrite;
    }
    // 0 is the filetype of rtc
    else if (dentry.filetype == 0){
        cur_pcb->file_descs[fd].file_operation->open_file = rtc_open;
        cur_pcb->file_descs[fd].file_operation->close_file = rtc_close;
        cur_pcb->file_descs[fd].file_operation->read_file = rtc_read;
        cur_pcb->file_descs[fd].file_operation->write_file = rtc_write;
    }
    else{
        cur_pcb->file_descs[fd].flags = 0;
        return -1;
    }
    return fd;
}

int32_t close(int32_t fd) {
    // cannot delete file whose fd is 0 or 1 because they are stdin and stdout
    if (fd <= 1 || fd >= MAX_OPENED_FILES)
        return -1;
    pcb_t* pcb = get_cur_pcb();
    pcb->file_descs[fd].flags = 0;
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

