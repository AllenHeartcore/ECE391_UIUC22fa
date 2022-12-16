#include <sys/types.h>
#include <sys/io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/mman.h>
#include <setjmp.h>
#include "mp1.h"

/* Extern the externally-visible MP1 functions */
extern int mp1_ioctl(unsigned long arg, unsigned long cmd);
extern void mp1_rtc_tasklet(unsigned long trash);

/* Global variable pointing to the base address of video memory
 * Used in the userspace tests to pass to mp1_rtc_tasklet */
char *vmem_base_addr;

sigjmp_buf segv_jmp_buf;

/* Imitates the cli instruction by blocking SIGIO
 * Used to help avoid synchronization problems between the ioctls
 * and the tasklet */
void fake_cli(void) {
	sigset_t old, new;
	sigemptyset(&new);
	sigaddset(&new, SIGIO);
	sigprocmask(SIG_BLOCK, &new, &old);
}

/* Imitates the sti instruction by unblocking SIGIO */
void fake_sti(void) {
	sigset_t old, new;
	sigemptyset(&new);
	sigaddset(&new, SIGIO);
	sigprocmask(SIG_UNBLOCK, &new, &old);
}
/* MP1 userspace signal handler */
void
__mp1_signal_handler(int garbage)
{
	/* Call the student's tasklet.  This will function nearly the same
	 * as in the kernel, since we are implementing the MP1 functionality
	 * as a tasklet */
	mp1_rtc_tasklet((unsigned long)&vmem_base_addr);
}
void
segv_handler(int garbage)
{
	siglongjmp(segv_jmp_buf, -1);
}

/* MP1 replacement for open system call */
int __mp1_open(char *file_path, int flags)
{
	int fd, fd_flags;

	/* Open the file */
#undef open
	fd = open(file_path, flags);
#define open __mp1_open

	/* If we're opening /dev/rtc, set up signal handling stuff
	 * for userspace testing */
	if(strcmp(file_path, "/dev/rtc") == 0) {

		/* SIGIO is the signal that will be sent to this process
		 * on an RTC interrupt.  Set __mp1_signal_handler to handle it */
		signal(SIGIO, __mp1_signal_handler);

		if( (fd_flags = fcntl(fd, F_GETFL, 0)) == -1) {
			perror("fcntl");
			return -1;
		}

		/* Set this process as the process to receive signals generated
		 * by the RTC interrupt */
		if(fcntl(fd, F_SETOWN, getpid()) == -1) {
			perror("fcntl");
			return -1;
		}

		/* The O_ASYNC flag sets the file descriptor to send a SIGIO
		 * signal when data is available to be "read".  In the case of
		 * the RTC driver, this signal will be sent to this process
		 * whenever the RTC generates an interrupt.  We can make use
		 * of this for userspace testing of the MP1 enhancements to
		 * the RTC driver */
		if(fcntl(fd, F_SETFL, fd_flags | O_ASYNC ) == -1) {
			perror("fcntl");
			return -1;
		}

	}

	return fd;
}

/* MP1 userspace ioctl function */
int __mp1_ioctl(int fd, unsigned long cmd, unsigned long arg)
{
	int ret;
	fake_cli();
	switch(cmd)
	{
		/* Pass the new ioctls to the student's MP1 ioctl function.
		 * This is exactly the same as in the kernel. */
		case RTC_STARTGAME:
			ret = mp1_ioctl(arg, 0);
			break;
		case RTC_ADDMISSILE:
			ret = mp1_ioctl(arg, 1);
			break;
		case RTC_MOVEXHAIRS:
			ret = mp1_ioctl(arg, 2);
			break;
		case RTC_GETSTATUS:
			ret = mp1_ioctl(arg, 3);
			break;
		case RTC_ENDGAME:
			ret = mp1_ioctl(arg, 4);
			break;
	/* Pass any other ioctls to the kernel via the actual system call */
		default:
			{
#undef ioctl
				return ioctl(fd, cmd, arg);
#define ioctl __mp1_ioctl
			}

	}
	fake_sti();
	return ret;
}

/* Userspace equivalents of copy_to_user and copy_from_user.
 * In userspace, these are just memcpys, because no translation is
 * necessary from kernel address space to user address space */
unsigned long mp1_copy_to_user (void *to, const void *from, unsigned long n)
{
	fake_sti();
	/* Turn on MP1 signal handling */
	signal(SIGSEGV, segv_handler);
	if(sigsetjmp(segv_jmp_buf,1) == 0) {
		memcpy(to, from, n);
		/* Turn off MP1 signal handling */
		signal(SIGSEGV, SIG_DFL);
		fake_cli();
		return 0;
	} else {
		/* Turn off MP1 signal handling */
		signal(SIGSEGV, SIG_DFL);
		fake_cli();
		return n;
	}
}
unsigned long mp1_copy_from_user (void *to, const void *from, unsigned long n)
{
	/* Turn on MP1 signal handling */
	fake_sti();
	signal(SIGSEGV, segv_handler);
	if(sigsetjmp(segv_jmp_buf,1) == 0) {
		memcpy(to, from, n);
		/* Turn off MP1 signal handling */
		signal(SIGSEGV, SIG_DFL);
		fake_cli();
		return 0;
	} else {
		/* Turn off MP1 signal handling */
		signal(SIGSEGV, SIG_DFL);
		fake_cli();
		return n;
	}
}

/* Wrappers around memory allocation.  This allows us to run
 * the mp1.S code in either userspace or the kernel */
void* mp1_malloc(int size)
{
	void *ret;
	fake_sti();
	ret = malloc(size);
	fake_cli();
	return ret;
}

void mp1_free(void *ptr)
{
	fake_sti();
	free(ptr);
	fake_cli();
}

/* In userspace, we can use the raise() function to send a signal to ourselves
 * Notice that this is an asynchronous notification, the handler is not called from this function */
void mp1_notify_user(void)
{
	//We use SIGUSR1 (user defined signal 1) here
	raise(SIGUSR1);
}
