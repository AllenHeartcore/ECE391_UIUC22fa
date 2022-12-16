
/* vga.c
 * A small set of functions to interface (from userspace) to the VGA in
 * text-mode.  
 * Mark Murphy 2007
 */

#include <unistd.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>

extern char* vmem_base_addr;
static void *mem_image = 0;

void clear_screen(){
	int i = 0;
	for(i = 0; i < 80*25; i++){
		((short*)mem_image)[i] = 0x720;
	}
}

void write_char(char c, int x, int y){
	((char*)mem_image)[(80*y + x)<<1] = c;
}

void write_string(char *s, int x, int y){
	int i = (80*y + x)<<1;
	while(*s){
		((char*)mem_image)[i] = *s++;
		i += 2;
	}
}

static void set_video_mode (void) {

	int fd = open ("/dev/mem", O_RDWR);

	if(ioperm (0,1024,1) == -1 || iopl (3) == -1) {
		perror ("set port permissions");
		return;
	}

	if(MAP_FAILED == (mem_image = mmap(0, (1<<20), PROT_READ | PROT_WRITE,
			      		   MAP_SHARED, fd, 0))){
		perror ("mmap low memory");
		return;
	}

	mem_image = mem_image + 0xb8000;
#ifdef _USERSPACE
	vmem_base_addr = mem_image;
#endif
	asm volatile(
		"movw  $0xC, %%ax\n"
		"\tmovw  $0x03d4, %%dx\n"
		"\toutw  %%ax,(%%dx)\n" 
		"\tmovw  $0x0d, %%ax\n"
		"\toutw  %%ax,(%%dx)\n"
		"\tmovw  $0xE, %%ax\n"
		"\toutw  %%ax,(%%dx)\n"
		"\tmovw  $0x0F,%%ax\n"
		"\toutw  %%ax,(%%dx)"::);
}

void init_screen(){
	set_video_mode();
	clear_screen();
}
