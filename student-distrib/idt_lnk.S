#include "idt_lnk.h"
# #include "keyboard.h"
# #include "rtc.h"

.text

.globl int_kbd_wrapper
.globl int_rtc_wrapper

int_kbd_lnk:
    pushal
    # call key_handler
    popal
    iret

int_rtc_lnk:
    pushal
    # call rtc_handler
    popal
    iret
