#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "filesys.h"

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
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int read_file_name_test() {
	TEST_HEADER;
	dentry_t test;
	uint8_t filename[FILE_NAME_MAX] = "frame0.txt";
	read_dentry_by_name(filename,&test);
	printf("The file's name is %s!\n",test.filename);

	if(strncmp((int8_t*)test.filename,(int8_t*)filename,FILE_NAME_MAX)!=0)
		return FAIL;
	return PASS;
}


/* Read by name test
 * 
 * Asserts that we can read data from data block
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int read_data_test() {
	// TEST_HEADER;
	dentry_t test;
	char buff[40000] = {'\0'};
	uint8_t filename[FILE_NAME_MAX] = "fish";
	int i;
	int32_t bytes_read;
	read_dentry_by_name(filename,&test);
	
	bytes_read = read_data(test.inode_num,0,(uint8_t*)buff,100000);
	clear();
	printf(buff);
	for(i=0; i <bytes_read; i++){
		if(buff[i]>=32 && buff[i]<=126)
			putc(buff[i]);
	}
	return PASS;
}







/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	/* The machine will FREEZE after an exception */
	// TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("page_test", page_test());
	// TEST_OUTPUT("page_test_deref_null", page_test_deref_null());
	// TEST_OUTPUT("page_test_deref_not_exist", page_test_deref_not_exist());
	// TEST_OUTPUT("div0_test", div0_test());
	// TEST_OUTPUT("read_file_name_test", read_file_name_test());
	TEST_OUTPUT("read_data_test", read_data_test());
}
