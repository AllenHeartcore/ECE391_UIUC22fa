#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

extern int32_t ece391_halt (uint8_t status);
extern int32_t ece391_execute (const uint8_t* command);
extern int32_t ece391_read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t ece391_write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t ece391_open (const uint8_t* filename);
extern int32_t ece391_close (int32_t fd);
extern int32_t ece391_getargs (uint8_t* buf, int32_t nbytes);
extern int32_t ece391_vidmap (uint8_t** screen_start);
extern int32_t ece391_set_handler (int32_t signum, void* handler);
extern int32_t ece391_sigreturn (void);

#endif
