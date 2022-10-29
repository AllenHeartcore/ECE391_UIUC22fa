#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

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


#endif
