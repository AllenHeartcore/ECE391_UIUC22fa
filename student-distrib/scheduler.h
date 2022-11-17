#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "types.h"
#include "filesys.h"
#include "terminal.h"

#define SCHEDULE_NUM 3
#define TERM_NOT_INIT -2  // This means the terminal hasn't been initialized yet
#define TERM_NOT_USE  -1  // This means one of the terminal has been closed (shell exit), no need to schedule
void scheduler();
/* Schedule array */
extern int32_t schedule_array[SCHEDULE_NUM];
extern int8_t  cur_sch_index;
#endif
