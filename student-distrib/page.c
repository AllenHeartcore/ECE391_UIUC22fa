#include "page.h"
#include "lib.h"
#include "syscall.h"	/* MAX_PROCESS, etc */
#include "terminal.h"

// 1024 entries, 2 entries used
// entry 0: 4m mapping, kernel
// entry 1: 4k mapping, video memory
pg_dir_t page_directory __attribute__((aligned (4 * 1024)));
// 1024 entries. 
pg_tbl_t page_table __attribute__((aligned (4 * 1024)));
/* Page table for video mapping */
pg_tbl_t video_page_table __attribute__((aligned (4 * 1024)));

/* Set a page directory entry according to given value */
#define SET_PDE(_pg_dir, _idx, _privilege, _ps, _global, _addr) do {    \
    (_pg_dir)[(_idx)].present = 1;                                      \
    (_pg_dir)[(_idx)].rw = 1;                                           \
    (_pg_dir)[(_idx)].privilege = (_privilege);                         \
    (_pg_dir)[(_idx)].pwt = 0;                                          \
    (_pg_dir)[(_idx)].pcd = 0;                                          \
    (_pg_dir)[(_idx)].accessed = 0;                                     \
    (_pg_dir)[(_idx)].dirty = 0;                                        \
    (_pg_dir)[(_idx)].ps = (_ps);                                       \
    (_pg_dir)[(_idx)].global = (_global);                               \
    (_pg_dir)[(_idx)].avl = 0;                                          \
    (_pg_dir)[(_idx)].addr = (_addr);                                   \
} while (0)

/* Set a page table entry according to given value */
#define SET_PTE(_pt, _idx, _privilege, _addr) do {  \
    (_pt)[(_idx)].present = 1;                      \
    (_pt)[(_idx)].rw = 1;                           \
    (_pt)[(_idx)].privilege = (_privilege);         \
    (_pt)[(_idx)].pwt = 0;                          \
    (_pt)[(_idx)].pcd = 0;                          \
    (_pt)[(_idx)].accessed = 0;                     \
    (_pt)[(_idx)].dirty = 0;                        \
    (_pt)[(_idx)].pat = 0;                          \
    (_pt)[(_idx)].global = 0;                       \
    (_pt)[(_idx)].avl = 0;                          \
    (_pt)[(_idx)].addr = (_addr);                   \
} while (0)

/* Update CR3 to flush TLB */
#define UPDATE_CR3() do {                   \
    asm volatile(                           \
        "movl %0, %%eax     \n\t"           \
        "movl %%eax, %%cr3  \n\t"           \
        :                                   \
        : "r" (&page_directory)             \
        : "%eax"                            \
    );                                      \
} while (0)

/*
*   page_init
*   initialize page directory table and page table, change some values in the registers
*   input: None
*   output: None
*   side effect: Change the value in the cr0, cr3, cr4 
*/

void page_init(void) {
    int i;
    // initialize all the entries in page directory table and page table
    for (i = 0; i < NUM_PG_TBL_ENTRY; i++){
        page_table[i].val = 0;
    }
    for (i = 0; i < NUM_PG_DIR_ENTRY; i++){
        page_directory[i].val = 0;
    }

    /* Set page table and page directory for video memory and backup buffer*/
    SET_PTE(page_table, VIDEO >> 12, 0, VIDEO >> 12);
    SET_PTE(page_table, (VIDEO >> 12) + 2, 0, (VIDEO >> 12) + 2);
    SET_PTE(page_table, (VIDEO >> 12) + 3, 0, (VIDEO >> 12) + 3);
    SET_PTE(page_table, (VIDEO >> 12) + 4, 0, (VIDEO >> 12) + 4);
    SET_PDE(page_directory, 0, 0, 0, 0, ((uint32_t) &page_table) >> 12);
    
    /* Set page directory for kernel page (4MB) */
    SET_PDE(page_directory, 1, 0, 1, 1, KERNEL_ADDR >> 12);


    // set the highest bit of cr0 to be 1
    // set the cr3 be the address of the page directory table
    // set the fifth bit of cr4 to be 1
    asm volatile(
        "movl  %0, %%eax;           \
         movl  %%eax, %%cr3;        \
         movl  %%cr4, %%eax;        \
         orl   $0x00000010, %%eax;  \
         movl  %%eax, %%cr4;        \
         movl  %%cr0, %%eax;        \
         orl   $0x80000000, %%eax;  \
         movl  %%eax, %%cr0;"
        : /*no output*/
        : "r" (&page_directory)
        : "%eax"
    );
}


/*
 *   set_user_prog_page
 *   Set page for a user program
 *   input: pid -- the pid of the user program
 *   output: None
 *   side effect: Change the paging directory; Change CR3; flush TLB
 */
void set_user_prog_page(uint32_t pid) {
    SET_PDE(page_directory, USER_SPACE >> 22, 1, 1, 0,
            (EIGHT_MB + pid*FOUR_MB) >> 12);
    UPDATE_CR3();
}

/*
 *   set_vidmap_page
 *   Set page for video memory
 *   input: screen_start -- starting address of the video memory
 *   output: None
 *   side effect: Change the paging directory; Change CR3; flush TLB
 */
void set_vidmap_page(uint8_t** screen_start) {
    
    /* Set page table and page directory to map USER_VIDEO_ADDR to
     * physical video memory (VIDEO) */
    SET_PDE(page_directory, USER_VIDEO_ADDR >> 22, 1, 0, 0,
            (uint32_t) &video_page_table >> 12);
    SET_PTE(video_page_table,
            (USER_VIDEO_ADDR & PG_TBL_NUMBER_MASK) >> 12, 1, VIDEO >> 12);

    UPDATE_CR3();

    /* Update screen_start */
    *screen_start = (uint8_t*) USER_VIDEO_ADDR;
}


/*
 *   remap_vidmap_page
 *   Set page for video memory for a specific terminal.
 *   If terminal_id = active one, directly write into physical memory.
 *   If terminal_id != active one, map user video memory to corresponding backup buffer.
 *   input: screen_start -- starting address of the video memory
 *   output: None
 *   side effect: Change the paging directory; Change CR3; flush TLB
 */
void remap_vidmap_page(uint8_t terminal_id) {

    if(terminal_id == current_term_id){
        SET_PTE(page_table, VIDEO >> 12, 0, VIDEO >> 12);
    }else{
        /* Set page table based on terminal_id */
        SET_PTE(page_table, VIDEO >> 12, 0, (VIDEO >> 12)+ 2 + terminal_id);
    }

    UPDATE_CR3();

}
