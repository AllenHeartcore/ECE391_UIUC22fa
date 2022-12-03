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
    int32_t return_address;
    int32_t cs;
    int32_t EFLAGS;
    int32_t ESP;
    int32_t SS;
} hwcontext

int default_signal_handler();
void send_signal(int sig_num);