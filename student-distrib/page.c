#include "page.h"
#include "lib.h"
// begin at 4M
#define KERNEL_ADDR (4*1024*1024)
#define VIDEO_PAGE_SIZE (4*1024)
// Start address is 0xB8000, end address is 0xC0000
#define VIDEO_PAGE_NUM ((0xC0000 - 0xB8000) / VIDEO_PAGE_SIZE)
// 1024 entries, 2 entries used
// entry 0: 4m mapping, kernel
// entry 1: 4k mapping, video memory
pg_dir_t page_directory __attribute__((aligned (4 * 1024)));
// 1024 entries. 
pg_tbl_t page_table __attribute__((aligned (4 * 1024)));


/*
*   page_init
*   initialize page directory table and page table, change some values in the registers
*   input: None
*   output: None
*   side effect: Change the value in the cr0, cr3, cr4 
*/

void page_init(void) {
    int i;
	uint32_t pd_pointer;
    // initialize all the entries in page directory table and page table
    for (i = 0; i < NUM_PG_TBL_ENTRY; i++){
        page_table[i].val = 0;
    }
    for (i = 0; i < NUM_PG_DIR_ENTRY; i++){
        page_directory[i].val = 0;
    }

    // set up the entry 0xB8 in page table for video memory
    // all these number are referenced in intel version 3
	page_table[VIDEO >> 12].present = 1;
	page_table[VIDEO >> 12].rw = 1;
	page_table[VIDEO >> 12].priviledge = 0;
	page_table[VIDEO >> 12].pwt = 0;
	page_table[VIDEO >> 12].pcd = 0;
	page_table[VIDEO >> 12].accessed = 0;
	page_table[VIDEO >> 12].dirty = 0;
	page_table[VIDEO >> 12].pat = 0;
	page_table[VIDEO >> 12].global = 0;
	page_table[VIDEO >> 12].avl = 0;
	// get the high 20 bits of mem
	page_table[VIDEO >> 12].addr = VIDEO >> 12;

    // set up the page directory entry for kernel
    page_directory[1].present = 1;
    page_directory[1].rw = 1;
    page_directory[1].priviledge = 0;
    page_directory[1].pwt = 0;
    page_directory[1].pcd = 1;
    page_directory[1].accessed = 0;
    page_directory[1].dirty = 0;
    page_directory[1].ps = 1;
    page_directory[1].global = 1;
    page_directory[1].avl = 0;
    // right shift for 22 because low 22 bits are all 0
    page_directory[1].addr = (KERNEL_ADDR >> 22) << 10;
    
    // set up the page directory entry for page table
    page_directory[0].present = 1;
    page_directory[0].rw = 1;
    page_directory[0].priviledge = 0;
    page_directory[0].pwt = 1;
    page_directory[0].pcd = 0;
    page_directory[0].accessed = 0;
    page_directory[0].dirty = 0;
    page_directory[0].ps = 0;
    page_directory[0].global = 0;
    page_directory[0].avl = 0;
    // get the high 20 bits for page_directory
	pd_pointer = (uint32_t) &page_table;
    page_directory[0].addr = pd_pointer >> 12;

    // set the highest bit of cr0 to be 1
    // set the cr3 be the address of the page directory table
    // set the fifth bit of cr4 to be 1
    __asm__ volatile(
        " movl  %0, %%eax;\
          movl  %%eax, %%cr3;\
          movl  %%cr4, %%eax; \
          orl   $0x00000010, %%eax;\
          movl  %%eax, %%cr4; \
          movl  %%cr0, %%eax;\
          orl   $0x80000000, %%eax; \
          movl  %%eax, %%cr0; \
        "
        : /*no output*/
        : "r" (&page_directory)
        : "%eax"
    );
}
