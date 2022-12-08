#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"
#include "filesys.h"
#include "terminal.h"
#include "scheduler.h"
#include "signal.h"

#define MAX_PROCESS 6          // At most 6 processes and...
#define MAX_OPENED_FILES 8     // 8 files can be concurrently opened
#define EIGHT_MB    0x800000   // 8 MB
#define FOUR_MB     0x400000   // 4 MB
#define EIGHT_KB    0x2000     // 8 KB
#define USER_SPACE  0x8000000  // 128 MB
#define USER_CODE   0x8048000  // starting point of user code
#define USER_STACK  0x8400000  // 132 MB

/* type specific file operations */
typedef int32_t(*read_ptr)(int32_t fd, void* buf, int32_t nbyte);
typedef int32_t(*write_ptr)(int32_t fd, const void* buf, int32_t nbyte);
typedef int32_t(*open_ptr)(const uint8_t*);
typedef int32_t(*close_ptr)(int32_t);

typedef struct file_operation
{
    open_ptr open_file;
    close_ptr close_file;
    read_ptr read_file;
    write_ptr write_file; 
} file_op;

/* file_descriptor entry */
typedef struct file_desc_t 
{
    file_op file_operation;
    uint32_t inode;
    int32_t file_position;
    int32_t flags;   // valid_desc: 0 invalid, 1 valid
} file_desc_t;

/* PCB */
typedef struct pcb
{
    file_desc_t file_descs[8];
    int signal[SIG_NUM];
    int sig_mask[SIG_NUM];
    void* signal_handler[SIG_NUM];
    uint32_t parent_pid;
    uint32_t cur_pid;
    int32_t ex_esp;  /* ESP for the process context when execute  */
    int32_t ex_ebp;  /* EBP for the process context when execute  */
    int32_t sch_ebp; /* EBP for the process context when schedule */
    int8_t args[KBD_BUF_SIZE + 1]; /* arguments passes to user programs */
} pcb_t;

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

pcb_t* get_pcb(uint32_t pid);
uint32_t get_cur_pid();
pcb_t* get_cur_pcb();


#endif
