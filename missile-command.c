
/* missile-command.c
 * Mark Murphy 2007
 * This is the user-space side of the ECE391 MP1 Missile Command game. Enjoy.
 */

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>

#include <sys/ioctl.h>
#include <sys/time.h>

#include "mp1.h"

/* screen writing routines  - defined in vga.c */
extern void init_screen();
extern void write_char(char, int x, int y);
extern void write_string(char*, int x, int y);
extern void clear_screen();

/* Static data */
static int rtc_fd = -1;		/* RTC file descriptor */
static int fired = 0;		/* Count of user-fired missiles */
volatile int score = 0;		/* score, as reported by ioctl(GETSTATUS) */
volatile int bases_left = 3;	/* number of bases remaining */


/* Input stuff */
static struct termios tio_orig;

/*  init_input()
 * Configure stdin for non-blocking, non-line-buffered, non-echoing input so
 * that the arrow-keys, spacebar, etc. behave correctly.
 */
int init_input() {
	struct termios tio_new;

	if (fcntl (fileno (stdin), F_SETFL, O_NONBLOCK) != 0) {
		perror ("fcntl to make stdin non-blocking");
		return -1;
	}

	if (tcgetattr (fileno (stdin), &tio_orig) != 0) {
		perror ("tcgetattr to read stdin terminal settings");
		return -1;
	}

	tio_new = tio_orig;
	tio_new.c_lflag &= ~(ICANON | ECHO);
	tio_new.c_cc[VMIN] = 1;
	tio_new.c_cc[VTIME] = 0;
	if (tcsetattr (fileno (stdin), TCSANOW, &tio_new) != 0) {
		perror ("tcsetattr to set stdin terminal settings");
		return -1;
	}

	return 0;
}

/* This command_t enum encodes the input keys we care about */
typedef enum { NOKEY, QUIT, LEFT, RIGHT, UP, DOWN, FIRE } command_t;

/* get_command()
 * Checks if a meaningful key was pressed, and returns it;
 */
command_t get_command(void) {
	char ch;
	int state = 0;
        while ((ch = getc (stdin)) != EOF) {
		switch(ch){
		    case '`': return QUIT;
		    case ' ': return FIRE;

		/* I am a vi-junkie, so you can control the crosshair with 
		 * the h,j,k,l vi-style cursor moving keys. */
		    case 'h': return LEFT;
		    case 'j': return DOWN;
		    case 'k': return UP;
		    case 'l': return RIGHT;
		}

		/* Arrow keys send the escape sequence "\033[?", where ? is one
		 * of A, B, C, or D . We use a small state-machine to track
		 * this character sequence */
		if (ch == '\033'){
		    state = 1; 
		}else if (ch == '[' && state == 1){
		    state = 2;
		}else {
			if (state == 2 && ch >= 'A' && ch <= 'D') {
				switch (ch) {
				    case 'A': return UP;
				    case 'B': return DOWN;
				    case 'C': return RIGHT;
				    case 'D': return LEFT;
				}
			}
			state = 0;
		}
	}
	return NOKEY;
}

/* make_missile()
 * Wrapper around the ioctl() to add a missile to the game.
 */
void make_missile(int sx, int sy, int dx, int dy, char c, int vel)
{
	struct missile m[1];
	int vx, vy, mag;

	m->x = (sx<<16) | 0x8000; 
	m->y = (sy<<16) | 0x8000;

	m->dest_x = dx;
	m->dest_y = dy;

	vx = (dx - sx);
	vy = (dy - sy);

	mag = sqrt((vx*vx + vy*vy)<<16);
	m->vx = ((vx<<16)*vel)/mag;
	m->vy = ((vy<<16)*vel)/mag;

	m->c = c;
	m->exploded = 0;

	ioctl(rtc_fd, RTC_ADDMISSILE, (unsigned long)m);
}

/* update_crosshairs()
 * move the crosshairs via the ioctl() 
 */
void update_crosshairs(command_t cmd){
	int dx = 0, dy = 0;
	static int chx = 80/2, chy = 25/2;

	switch(cmd){
	    case LEFT:  dx--; break;
	    case RIGHT: dx++; break;
	    case UP:    dy--; break;
	    case DOWN:  dy++; break;
	    case FIRE:  
		make_missile(79, 24, chx, chy, '*', 200);
		fired++;
	    default:
		break;
	}

	if((dx != 0) || (dy != 0)){
		unsigned long d;

		if(80 <= (chx += dx))
			chx = 79;
		if(0 > chx)
			chx = 0;
		if(24 <= (chy += dy))
			chy = 24;
		if(0 > chy)
			chy = 0;

		d = (unsigned long)dx&0xFFFF;
		d |= ((unsigned long)dy&0xFFFF)<<16;

		ioctl(rtc_fd, RTC_MOVEXHAIRS, d);
	}

}

void siginthandler(int ignore){
    /* Reset I/O in desperation, then let signal do its usual thing... */
    tcsetattr (fileno (stdin), TCSANOW, &tio_orig);
    signal (SIGINT, SIG_DFL);
    kill (getpid (), SIGINT);
}

void sigusr1_handler(int foobar){
	unsigned long status_word;
	if(!ioctl(rtc_fd, RTC_GETSTATUS, (unsigned long) &status_word)){
		score = status_word&0xFFFF;
		bases_left = ((status_word>>16)&1) + ((status_word>>17)&1)
				+ ((status_word>>18)&1);
		
	}
}

void draw_centered_string(char *s, int y){
	write_string(s, (80-strlen(s))/2, y);
}

#define DCS(str) draw_centered_string( str , line++)

void draw_starting_screen(){
	int line = 5;
	clear_screen();
	DCS("                       MISSILE COMMAND                          ");
	DCS("                      Mark Murphy, 2007                         ");
	DCS("                                                                ");
	DCS("                          Commands:                             ");
	DCS("               space ................. fire missile             ");
	DCS("          arrow keys ................. move crosshairs          ");
	DCS("             h,j,k,l ................. move crosshairs (vi-style");
	DCS("        ` (backtick) ................. exit the game            ");
	DCS("                                                                ");
	DCS("                                                                ");
	DCS("   Protect your bases by destroying the enemy missiles (e's)    ");
	DCS("   with your missiles. You get 1 point for each enemy           ");
	DCS("   missile you destroy. The game ends when your bases are all   ");
	DCS("   dead or you hit the ` key.                                   ");
	DCS("                                                                ");
	DCS("               Press the space bar to continue.                 ");
}

void draw_status_bar(){
	char buf[80];
	int percent = fired ? (100*score)/fired : 0;
	snprintf(buf, 80, "[score %3d] [fired %3d] [accuracy %3d%%]   ", 
				score, fired, percent);
	write_string(buf, 0, 0);
}

int main(){
	command_t cmd;
	struct timeval tv[1], last[1];
	unsigned long ival, avg_ival, elapsed;
	int count = 0;

	/* Set up the random enemy missile generation */
	avg_ival = 4000000; /* Begin the interval at 4 seconds */
	ival     = avg_ival + ((rand()%1000000) - 500000);
	gettimeofday(last, NULL);

	/* Initialize the RTC */

	if(-1 == (rtc_fd = open("/dev/rtc", O_RDWR))){
		perror("/dev/rtc");
		return -1;
	}

	signal(SIGUSR1, sigusr1_handler);
	ioctl(rtc_fd, RTC_STARTGAME, 0);

	/* Try not to leave terminal in unusable state when terminating... */
	signal(SIGINT, siginthandler);

	/* Some other initialization ... */
	init_screen();
	init_input();

	/* On with the game! */
	draw_starting_screen();
	while(FIRE != get_command());
	clear_screen();

	/* Start the RTC running */
	ioctl(rtc_fd, RTC_IRQP_SET, 32);
	ioctl(rtc_fd, RTC_PIE_ON, 0);

	while(bases_left && QUIT != (cmd = get_command())) {
		draw_status_bar();
		update_crosshairs(cmd);

		gettimeofday(tv, NULL);

		tv->tv_sec -= last->tv_sec;
		if((tv->tv_usec -= last->tv_usec) < 0){
			tv->tv_sec--;
			tv->tv_usec += 1000000;
		}

		elapsed = tv->tv_sec*1000000 + tv->tv_usec;

		/* Inject enemy missiles after random amounts of time */
		if(ival <= elapsed){
			/* The longer the game runs, the more frequent the
			 * enemy missiles come */
			if(!((++count)%10) && avg_ival > 200000){
				avg_ival -= 100000;
			}
			ival = avg_ival + ((rand()%1000000) - 500000);

			make_missile(rand()%80, 0, 20*(rand()%3+1), 24, 'e',
					rand()%5 + 10);

			gettimeofday(last, NULL);
		}
		
	}

	/* Shutdown the RTC */
	ioctl(rtc_fd, RTC_ENDGAME, 0);
	ioctl(rtc_fd, RTC_PIE_OFF, 0);
	close(rtc_fd);

	draw_centered_string("+--------------------------------+", (25/2)-1);
	draw_centered_string("| Game over. Press space to exit |",  25/2);
	draw_centered_string("+--------------------------------+", (25/2)+1);

	while(FIRE != get_command());

	clear_screen();
    	tcsetattr (fileno (stdin), TCSANOW, &tio_orig);

	printf("\nGame over. Your score was %d\n\n", score);

	return 0;
}
