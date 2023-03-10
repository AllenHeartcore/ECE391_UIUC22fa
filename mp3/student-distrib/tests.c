#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "filesys.h"
#include "memory.h"
#include "syscall.h"
#include "ata.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* Div 0 Test
 * 
 * Test if div 0 exception can be triggered
 * should show exception message, return
 * FAIL if divide 0 without exception.
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 * Coverage: Div 0 exception (IDT)
 * Files: idt.c/h
 */
int div0_test(){
	TEST_HEADER;

	int a = 1;
	int b = 0;
	int c = a / b;
	c++;	/* to suppress the "unused variable" warning */

	return FAIL;
}

/* Page test
 * 
 * Dereference each address in video memory and kernel space,
 * return PASS if successfully dereference all addresses
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging
 * Files: page.c/h
 */
int page_test() {
	TEST_HEADER;
	unsigned char* video_mem_start = (unsigned char*) 0xB8000;
	unsigned char* video_mem_end   = video_mem_start + 0x1000;
	unsigned char* kernel_start    = (unsigned char*) 0x400000;
	unsigned char* kernel_end      = (unsigned char*) 0x800000;
	unsigned char  foo;
	unsigned char* addr;

	for (addr = video_mem_start; addr < video_mem_end; addr++) {
		foo = (*addr);
	}

	for (addr = kernel_start; addr < kernel_end; addr++) {
		foo = (*addr);
	}

	return PASS;
}

/* Page test (dereference NULL)
 * 
 * Dereference NULL to check if page fault exception can be triggered
 * return FAIL if successfully dereference NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging
 * Files: page.c/h
 */
int page_test_deref_null() {
	TEST_HEADER;

	uint8_t foo;
	uint8_t *null = NULL;

	foo = *null;

	return FAIL;
}

/* Page test (dereference address not exist)
 * 
 * Dereference an address that is not exist
 * return FAIL if successfully dereference the not-exist address
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging
 * Files: page.c/h
 */
int page_test_deref_not_exist() {
	TEST_HEADER;

	uint8_t foo;
	uint8_t *addr_not_exist = (uint8_t*)0x800001;
	foo = *addr_not_exist;

	return FAIL;
}

/* Checkpoint 2 tests */



/* Read by name test
 * 
 * Asserts that we can read dentry by name
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int read_file_name_test(uint8_t* filename) {
	TEST_HEADER;
	dentry_t test;
	printf("READ FILE TEST");
	printf(" \n");
	if (read_dentry_by_name(filename,&test) == -1)
		return FAIL;
	printf("The file's name is %s!\n",test.filename);

	if(strncmp((int8_t*)test.filename,(int8_t*)filename,FILE_NAME_MAX)!=0)
		return FAIL;
	return PASS;
}

/* Access data by name test
 * 
 * Asserts that we can read data from data block
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int read_data_test(uint8_t* filename) {
	// TEST_HEADER;
	dentry_t test;
	char buff[40000] = {'\0'};
	int i;
	int32_t bytes_read;
	read_dentry_by_name(filename,&test);
	bytes_read = read_data(test.inode_num,0,(uint8_t*)buff,100000);
	for(i=0; i <bytes_read; i++)
		putc(buff[i]);
	return PASS;
}



/* Read directory test
 * 
 * Asserts that we can read directory
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Read directory
 */
int read_directory_test(){
	TEST_HEADER;
	int i;
	uint8_t buf[4096];
	buf[32] = '\0';
	// 63 is the max directory number in filesystem
	for (i = 0; i < 63; i++){
		if (read_directory(buf, i) == -1)
			break;
		printf((int8_t*)buf);
		printf("\n");
	}
	return PASS;
}

/*
* File Open test
* Asserts we can open the file
* Inputs: None
* Outputs: PASS/FAIL
* Side Effects: None
* Coverage: Open the file
*/
int open_file_test(uint8_t* filename){
	TEST_HEADER;
	if (fopen(filename) == -1)
		return FAIL;
	return PASS;
}

/*
* File Close test
* Asserts we can close the file
* Inputs: None
* Outputs: PASS/FAIL
* Side Effects: None
* Coverage: Close the file
*/
int close_file_test(){
	TEST_HEADER;
	int fd = 0;
	if (fclose(fd) == 0)
		return PASS;
	return FAIL;
}

/*
* File Write test
* Asserts we can write the file
* Inputs: None
* Outputs: PASS/FAIL
* Side Effects: None
* Coverage: Write the file
*/
int write_file_test(uint8_t *filename, uint8_t *buf, uint32_t len){
	TEST_HEADER;
	// printf("[TEST]: rying to write %d bytes \"%s\" to file %s\n",
	// 	len, buf, filename);
	int32_t fd = -1;
	int32_t bytes_written;
	uint8_t read_buf[1024] = {'\0'};
	/* first open a file */
	fd = open(filename);
	if (-1 == fd)
		return FAIL;
	bytes_written = write(fd, buf, len);
	// printf("[TEST]: bytes written: %d\n", bytes_written);
	// read(fd, read_buf, 1024);
	// printf("[TEST]: contents of the file after writing:\n");
	// printf("%s\n", read_buf);
	return bytes_written;
}

/*
* File Read test
* Asserts we can read the file
* Inputs: None
* Outputs: PASS/FAIL
* Side Effects: None
* Coverage: Read the file
*/

int read_file_test(){
	TEST_HEADER;
	if (fread(0, NULL, 0) == 0)
		return PASS;
	return FAIL;
}




/* RTC Driver Test
 *
 * Changing through all possible RTC frequencies
 * and print out a character for every interrupt
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: RTC driver
 * Files: rtc.c/h
 */
int rtc_driver_test() {
	TEST_HEADER;

	int freq, rate, count;
	for (freq = 2, rate = 0; freq <= 1024; freq <<= 1, rate++) {
		rtc_write(0, &freq, 4);
		for (count = 0; count < freq; count++) {
			rtc_read(0, NULL, 0);
			printf("%d", rate);
		}
		printf("\n");
	}

	return PASS;
}

/* Terminal Keyboard Test (Echo)
 * 
 * Try to repeatedly read from and write to terminal
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Terminal, Keyboard
 * Files: terminal.c/h, keyboard.c/h
 */
int terminal_kbd_test_echo(int32_t read_nbytes, int32_t write_nbytes) {
	TEST_HEADER;

	int32_t retr, retw;
	uint8_t buf[128];
	while (1) {
		retr = terminal_read(0, buf, read_nbytes);
		retw = terminal_write(0, buf, write_nbytes);
		printf("Read %d bytes, wrote %d bytes\n", retr, retw);
	}

	return PASS;
}

/* Terminal Keyboard Test (Newline)
 * 
 * Check if the terminal automatically wraps to the next line
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Terminal, Keyboard
 * Files: terminal.c/h, keyboard.c/h
 */
int terminal_kbd_test_newline(int32_t write_nbytes) {
	TEST_HEADER;

	uint8_t buffer[100] = {
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	};
	terminal_init();
	terminal_write(0, buffer, write_nbytes);

	return PASS;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

int ata_read_test() {
	TEST_HEADER;
	uint32_t i, j;
	uint8_t flag;
	uint8_t buf[512*256] = { 0 };
	printf("buf created:\n");
	for (i = 0; i < 512; i++) {
		printf("%x", buf[i]);
	}
	printf("\n");
	flag = ata_read_pio28(1, 0, buf);
	printf("buf after reading:\n");
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 512; j++) {
			printf("%x", buf[i*512+j]);
		}
		printf("\n");
	}
	return flag;
}

int ata_write_test() {
	TEST_HEADER;
	uint32_t i, j;
	uint8_t flag;
	uint8_t buf_read[ATA_SECTOR_SIZE] = { 0 };
	uint8_t buf_write[ATA_SECTOR_SIZE];
	uint32_t target_sec = 5000;

	ata_read_pio28(target_sec, 1, buf_read);
	printf("buf before writing:\n");
	for (i = 0; i < ATA_SECTOR_SIZE; i++) {
		printf("%x", buf_read[i]);
	}
	printf("\n");

	for (j = 0; j < ATA_SECTOR_SIZE; j += 4) {
		buf_write[j] = 0x12;
		buf_write[j+1] = 0x34;
		buf_write[j+2] = 0x56;
		buf_write[j+3] = 0x78;
	}

	flag = ata_write_pio28(target_sec, 1, buf_write);
	printf("reading ...\n");
	ata_read_pio28(target_sec, 1, buf_read);
	printf("buf after writing:\n");
	for (i = 0; i < ATA_SECTOR_SIZE; i++) {
		printf("%x", buf_read[i]);
	}
	printf("\n");
	return flag;
}


/* Memory allocation test
 * 
 * Check if the terminal automatically wraps to the next line
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Terminal, Keyboard
 * Files: terminal.c/h, keyboard.c/h
 */

int memory_allocation_test() {
	TEST_HEADER;
	int32_t i,flag;
	void* ptr;
	flag = 1;
	for(i=0; i<100000; i++){
		ptr = malloc_fixlen(&slab_cache_list);
		if(free_fixlen(&slab_cache_list,ptr) == 0)
			flag = 0;
	}
	i = 100;
	if(free_fixlen(&slab_cache_list, (void*)i) == 1)
		flag = 0;


	void *p1,*p2,*p3;
	p1 = malloc_varlen(100);
	p2 = malloc_varlen(200);
	p3 = malloc_varlen(300);
	
	free_varlen(p1);
	free_varlen(p2);
	free_varlen(p3);
	

	if(flag)
		return PASS;
	return FAIL;
}

int slab_cache_test(){
	TEST_HEADER;
	char name[20] = {"TestCache"};
	slab_cache* test = slab_cache_create(name, 32);
	int32_t i;
	int32_t* ptr;
	for(i = 0; i <500; i++){
		ptr = (int32_t*)slab_cache_alloc(test);
		slab_cache_free(test, ptr);
	}
	slab_cache_destroy(test);

	char name2[20] = {"TestCache2"};
	slab_cache* test2 = slab_cache_create(name2, 32);
	slab_cache_destroy(test2); // Should succeed

	char name3[20] = {"TestCache3"};
	slab_cache* test3 = slab_cache_create(name3, 32);
	slab_cache_alloc(test3);
	slab_cache_destroy(test3); // Should fail and print error

	return PASS;
}



/* Test suite entry point */
void launch_tests(){
	/* Checkpoint 1 tests */
	/* The machine will FREEZE after an exception */
	// // TEST_OUTPUT("idt_test", idt_test());
	// // TEST_OUTPUT("page_test", page_test());
	// // TEST_OUTPUT("page_test_deref_null", page_test_deref_null());
	// // TEST_OUTPUT("page_test_deref_not_exist", page_test_deref_not_exist());
	// // TEST_OUTPUT("div0_test", div0_test());

	/* Checkpoint 2 tests */
	// TEST_OUTPUT("open_file_test", open_file_test((uint8_t*)"nonexistant_filename"));
	// sleep(1);
	// TEST_OUTPUT("close_file_test", close_file_test());
	// sleep(1);
	// TEST_OUTPUT("read_file_test", read_file_test());
	// sleep(1);
	// TEST_OUTPUT("write_file_test", write_file_test("frame0.txt", "sb", 2));
	// sleep(1);
	// TEST_OUTPUT("read_file_name_test", read_file_name_test((uint8_t*)"frame0.txt"));
	// sleep(4);
	// TEST_OUTPUT("read_data_test", read_data_test((uint8_t*)"pingpong"));
	// sleep(4);
	// TEST_OUTPUT("read_directory", read_directory_test());
	// sleep(4);
	// TEST_OUTPUT("rtc_driver_test", rtc_driver_test());
	// sleep(4);
	// TEST_OUTPUT("terminal_kbd_test_echo", terminal_kbd_test_echo(128, 128));
	// TEST_OUTPUT("terminal_kbd_test_newline", terminal_kbd_test_newline(100));

	/* Checkpoint 3 tests */
	// uint8_t cmd[128] = "ls";
	// asm volatile("movl %0, %%ebx; \n\
	// 			  movl $1, %%eax; \n\
	// 			  int $0x80;"
	// 			 :
	// 			 : "r" (cmd)
	// 			 : "eax", "ebx"
	// 			 );
	TEST_OUTPUT("ata test", ata_write_test());
}
