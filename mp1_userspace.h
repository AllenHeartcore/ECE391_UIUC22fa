#ifndef _ASM
int __mp1_test_init(void);
void __mp1_signal_handler(int garbage);
int __mp1_ioctl(int fd, unsigned long cmd, unsigned long arg);
unsigned long __mp1_copy_to_user (void * to, const void * from, unsigned long n);
unsigned long __mp1_copy_from_user (void * to, const void * from, unsigned long n);
int __mp1_open(char *filepath, int flags);
#endif
