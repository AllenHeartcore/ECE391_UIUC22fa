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
    pushl %esp      # the order matters
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax
    cmpl $0, %eax
    jl invalid_callnum
    cmpl $9, %eax
    jg invalid_callnum
    jmp *syscall_jmptbl(, %eax, 4)
invalid_callnum:    # maybe we should raise an exception
    popl %eax
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %esp
    popfl
    iret
