#include "page.h"
#include "lib.h"

#define KERNEL_ADDR (4*1024*1024)
#define VIDEO_PAGE_SIZE (4*1024)
#define VIDEO_PAGE_NUM ((0xC0000 - 0xB8000) / VIDEO_PAGE_SIZE)
// 1024 entries, 2 entries used
// entry 0: 4m mapping, kernel
// entry 1: 4k mapping, video memory
pg_dir_t page_directory __attribute__((aligned (4 * 1024)));
// 1024 entries. 
pg_tbl_t page_table __attribute__((aligned (4 * 1024)));



void page_init(void) {
    int i;
    // initialize all the entries in page directory table and page table
    for (i = 0; i < NUM_PG_DIR_ENTRY; i++){
        page_table[i].val = 0;
    }
    for (i = 0; i < NUM_PG_TBL_ENTRY; i++){
        page_directory[i].val = 0;
    }

    // set up the entries in page table for video memory
    for (i = 0; i < VIDEO_PAGE_NUM; i++){
        page_table[i].present = 1;
        page_table[i].rw = 1;
        page_table[i].priviledge = 0;
        page_table[i].pwt = 0;
        page_table[i].pcd = 0;
        page_table[i].accessed = 0;
        page_table[i].dirty = 0;
        page_table[i].pat = 0;
        page_table[i].global = 0;
        page_table[i].avl = 0;
        // get the high 20 bits of mem
        page_table[i].addr = (i * VIDEO_PAGE_SIZE + VIDEO) >> 12;
    }

    // set up the page directory entry for kernel
    page_directory[0].present = 1;
    page_directory[0].rw = 1;
    page_directory[0].priviledge = 0;
    page_directory[0].pwt = 0;
    page_directory[0].pcd = 1;
    page_directory[0].accessed = 0;
    page_directory[0].dirty = 0;
    page_directory[0].ps = 1;
    page_directory[0].global = 1;
    page_directory[0].avl = 0;
    // right shift for 22 because low 22 bits are all 0
    page_directory[0].addr = (KERNEL_ADDR >> 22) << 10;
    
    // set up the page directory entry for page table
    page_directory[1].present = 1;
    page_directory[1].rw = 1;
    page_directory[1].priviledge = 0;
    page_directory[1].pwt = 1;
    page_directory[1].pcd = 0;
    page_directory[1].accessed = 0;
    page_directory[1].dirty = 0;
    page_directory[1].ps = 0;
    page_directory[1].global = 0;
    page_directory[1].avl = 0;
    // get the high 20 bits for page_directory
    page_directory[1].addr = &page_table >> 12;

    __asm__ volatile(
        // "movl   %1, %%cr3; \
        //  orl    $0x80000001, %%cr0; \
        //  orl    $0x8, %%cr4; \
        //  andl   $FFFFFFEF %%cr4; \
        //  movl   $1, %0\
        // "
        // change the value of cr0, cr3, cr4
        " movl  %1, %%eax;\
          movl  %%eax, %%cr3;\
          movl  %%cr4, %%eax; \
          orl   $0x00000010, %%eax;\
          movl  %%eax, %%cr4; \
          movl  %%cr0, %%eax;\
          orl   $0x80000001, %%eax; \
          movl  %%eax, %%cr0; \
        "
        : /*no output*/
        : "r" (&page_directory)
        : "%cr0", "%cr3", "%cr4", "%eax"
    )
}
