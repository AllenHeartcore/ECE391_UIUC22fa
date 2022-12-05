#define ASM

.globl int_kbd_lnk
.globl int_rtc_lnk
.globl syscall_lnk
.global pit_handler_linkage
.global exc_pft_linkage
.global exc_dv0_linkage
.global exc_dbg_linkage
.global exc_nmi_linkage
.global exc_bpt_linkage
.global exc_ovf_linkage
.global exc_bre_linkage
.global exc_ivo_linkage
.global exc_dna_linkage
.global exc_dbf_linkage
.global exc_cso_linkage
.global exc_its_linkage
.global exc_snp_linkage
.global exc_ssf_linkage
.global exc_gpf_linkage
.global exc_res_linkage
.global exc_fpe_linkage
.global exc_alc_linkage
.global exc_mac_linkage
.global exc_sfe_linkage

pit_handler_linkage:
    addl $-4, %esp      # push dummy in error code slot
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call pit_handler
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp           # pop out the error code
    iret

int_kbd_lnk:
    addl $-4, %esp      # push dummy in error code slot
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call key_handler
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp           # pop out the error code
    iret

int_rtc_lnk:
    addl $-4, %esp      # push dummy in error code slot
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call rtc_handler
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp           # pop out the error code
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
    addl $-4, %esp      # push dummy in error code slot
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx

    cmpl $1, %eax   # sanity check
    jl invalid_callnum
    cmpl $10, %eax
    jg invalid_callnum

    pushl %edx      # push arguments
    pushl %ecx
    pushl %ebx
    cmpl $10, %eax
    je handle_sigreturn
    call *syscall_jmptbl(, %eax, 4)
    addl $12, %esp
    jmp syscall_done

handle_sigreturn:
    call *syscall_jmptbl(, %eax, 4)
    addl $12, %esp
    jmp  sigreturn_done

invalid_callnum:
    movl $-1, %eax

syscall_done:
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    addl $4, %esp
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp           # pop out the error code
    iret

sigreturn_done:
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp           # pop out the error code
    iret


exc_dv0_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_dv0
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_dbg_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_dbg
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_nmi_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_nmi
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_bpt_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_bpt
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_ovf_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_ovf
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_bre_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_bre
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_ivo_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_ivo
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_dna_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_dna
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl  $4, %esp
    iret

exc_dbf_linkage:
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_dbf
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_cso_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_cso
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_its_linkage:
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_its
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_snp_linkage:
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_snp
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_ssf_linkage:
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_ssf
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_gpf_linkage:
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_gpf
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_pft_linkage:
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_pft
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp           # pop out the error code
    iret

exc_res_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_res
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_fpe_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_fpe
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_alc_linkage:
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_alc
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_mac_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_mac
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

exc_sfe_linkage:
    addl  $-4, %esp
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    call exc_sfe
    call handle_signal
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret

