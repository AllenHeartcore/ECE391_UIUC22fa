#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"
#include "filesys.h"

#define MAX_OPENED_FILES 8

extern int32_t halt (uint8_t status);
extern int32_t execute (const uint8_t* command);
extern int32_t read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open (const uint8_t* filename);
extern int32_t close (int32_t fd);
extern int32_t getargs (uint8_t* buf, int32_t nbytes);
extern int32_t vidmap (uint8_t** screen_start);
extern int32_t set_handler (int32_t signum, void* handler);
extern int32_t sigreturn (void);



/* type specific file operations */
typedef int32_t(*read_ptr)(const uint8_t*);
typedef int32_t(*write_ptr)(int32_t fd, const void* buf, int32_t nbyte);
typedef int32_t(*open_ptr)(const uint8_t*);
typedef int32_t(*close_ptr)(int32_t);



// typedef struct reg_file_op
// {
//     int32_t (*open_file)(const uint8_t* filename);
//     int32_t (*close_file)(int32_t fd);
//     int32_t (*read_file)(int32_t fd, void* buf, int32_t nbytes);
//     int32_t (*write_file)(int32_t fd, void* buf, int32_t nbytes);
// } reg_file_op_t;

// typedef struct rtc_op
// {
//     int32_t (*open_rtc)(const uint8_t* filename);
//     int32_t (*close_rtc)(int32_t fd);
//     int32_t (*read_rtc)(int32_t fd, void* buf, int32_t nbytes);
//     int32_t (*write_rtc)(int32_t fd, void* buf, int32_t nbytes);
// } rtc_op_t;

// typedef struct dir_op
// {
//     int32_t (*open_directory)(const uint8_t* filename);
//     int32_t (*close_directory)(int32_t fd);
//     int32_t (*read_directory)(int32_t fd, void* buf, int32_t nbytes);
//     int32_t (*write_directory)(int32_t fd, void* buf, int32_t nbytes);
// } dir_op_t;

// typedef struct stdin_op
// {
//     int32_t (*open_terminal)(const uint8_t* filename);
//     int32_t (*close_terminal)(int32_t fd);
//     int32_t (*read_terminal)(int32_t fd, void* buf, int32_t nbytes);
//     int32_t (*write_stdout)(int32_t fd, void* buf, int32_t nbytes);
// } stdin_op_t;

// typedef struct stdout_op
// {
//     int32_t (*open_terminal)(const uint8_t* filename);
//     int32_t (*close_terminal)(int32_t fd);
//     int32_t (*read_stdout)(int32_t fd, void* buf, int32_t nbytes);
//     int32_t (*write_terminal)(int32_t fd, void* buf, int32_t nbytes);
// } stdout_op_t;


typedef struct file_operation
{
    open_ptr open_file;
    close_ptr close_file;
    read_ptr read_file;
    write_ptr write_file; 
}file_op;




/* flag structure */
// 0 for rtc, 1 for directory, 2 fir regular file, 3 for standard in, 4 for standard out.
typedef struct flag
{
    int16_t filetype;
    int16_t valid_desc;
} flag_t;

/* file_descriptor entry */

typedef struct file_desc_t 
{
    file_op* file_operation;
    inode_t* inode;
    int32_t file_position;
    flag_t flags;
} file_desc_t;



/* PCB */

typedef struct pcb
{
    file_desc_t file_descs[8];
    int32_t signal;
    uint32_t parent_pid;
    uint32_t cur_pid;
    int32_t ex_esp; /* ESP for the process context when execute */
    int32_t ex_ebp; /* EBP for the process context when execute */
} pcb_t;


/* Process id array */
uint32_t pid_array[6];

#endif
