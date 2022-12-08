#define ASM

DO_SIG_RETURN:
    movl    $10, %eax
    int     0x80
    RET
DO_SIG_RETURN_END:



    