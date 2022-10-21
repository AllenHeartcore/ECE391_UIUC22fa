#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"

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

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	/* Checkpoint 1 tests */
	/* The machine will FREEZE after an exception */
	// TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("page_test", page_test());
	// TEST_OUTPUT("page_test_deref_null", page_test_deref_null());
	// TEST_OUTPUT("page_test_deref_not_exist", page_test_deref_not_exist());
	// TEST_OUTPUT("div0_test", div0_test());

	/* Checkpoint 2 tests */
	TEST_OUTPUT("rtc_driver_test", rtc_driver_test());
}
