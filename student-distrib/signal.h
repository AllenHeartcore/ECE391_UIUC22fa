#ifndef SIGNAL_H
#define SIGNAL_H

#define SIG_DIV_ZERO    0
#define SIG_SEGFAULT    1
#define SIG_INTERRUPT   2
#define SIG_ALARM       3
#define SIG_USER1       4
#define SIG_NUM         5
#define SIG_DEACTIVATE  0
#define SIG_ACTIVATE    1
#define SIG_UNMASK      0
#define SIG_MASK        1



typedef struct hwcontext {
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t eax;
    uint32_t ds;
    uint32_t es;
    uint32_t fs;
    uint32_t error_code;
    uint32_t return_address;
    uint32_t cs;
    uint32_t EFLAGS;
    uint32_t ESP;
    uint32_t SS;
} hwcontext;

extern int default_signal_handler();
extern void send_signal(int sig_num);
extern void handle_signal();
extern void DO_SIG_RETURN(void);
extern void DO_SIG_RETURN_END(void);
extern void kill_task(void);
extern void ignore(void);

#endif
