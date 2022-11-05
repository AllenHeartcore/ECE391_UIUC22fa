#define ASM

.globl int_kbd_lnk
.globl int_rtc_lnk
.globl syscall_lnk

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

syscall_jmptbl:
    .long 0         # Syscall number 0 is reserved
    .long halt
    .long execute
    .long read
    .long write
    .long open
    .long close
    .long getargs
    .long vidmap
    .long set_handler
    .long sigreturn

syscall_lnk:
    pushfl
    pushl %edi
    pushl %esi
    pushl %edx      # callee-save (not args)
    pushl %ecx
    pushl %ebx

    cmpl $0, %eax   # sanity check
    jl invalid_callnum
    cmpl $9, %eax
    jg invalid_callnum

    pushl %edx      # push arguments
    pushl %ecx
    pushl %ebx
    call *syscall_jmptbl(, %eax, 4)
    addl $12, %esp

invalid_callnum:    # maybe we should raise an exception
    popl %ebx       # We don't save and restore eax, ...
    popl %ecx       # because it holds the return value
    popl %edx
    popl %esi
    popl %edi
    popfl
    iret
