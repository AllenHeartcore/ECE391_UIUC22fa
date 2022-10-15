#define ASM

.globl int_kbd_lnk
.globl int_rtc_lnk

int_kbd_lnk:
    pushal
    pushfl
    call key_handler
    popfl
    popal
    iret

int_rtc_lnk:
    pushal
    pushfl
    call rtc_handler
    popfl
    popal
    iret
