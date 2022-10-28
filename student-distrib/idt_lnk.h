#ifndef IDT_LNK_H
#define IDT_LNK_H

#include "keyboard.h"
#include "rtc.h"
#include "syscall.h"

extern void int_kbd_lnk(void);
extern void int_rtc_lnk(void);
extern void syscall_lnk(void);

#endif
