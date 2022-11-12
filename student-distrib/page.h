/* page.h - Defines operations and structure for paging
 * vim:ts=4 noexpandtab
 */

#ifndef _PAGE_H
#define _PAGE_H

#include "syscall.h"

/* Total number of page directory
 * entries and page table entries */
#define NUM_PG_DIR_ENTRY 1024
#define NUM_PG_TBL_ENTRY 1024

/* Virtual address of user programs */
#define USER_VIDEO_ADDR (USER_SPACE + FOUR_MB)
#define PG_TBL_NUMBER_MASK 0x003FF000

#include "types.h"

/* Page directory entry, each entry points to a page table */
typedef union pg_dir_entr_t {
	uint32_t val;
	struct {
		uint32_t present    :  1;  /* 1-bit present flag            */
		uint32_t rw         :  1;  /* 1-bit read / write flag       */
		uint32_t privilege  :  1;  /* 1-bit, user / supervisor      */
		uint32_t pwt        :  1;  /* 1-bit write through flag      */
		uint32_t pcd        :  1;  /* 1-bit cache disable flag      */
		uint32_t accessed   :  1;  /* 1-bit accessed flag           */
		uint32_t dirty      :  1;  /* 1-bit dirty flag              */
		uint32_t ps         :  1;  /* 1-bit page size (0)           */
		uint32_t global     :  1;  /* 1-bit global flag             */
		uint32_t avl        :  3;  /* 4-bit available flag          */
		uint32_t addr       : 20;  /* 20-bit page table base addr   */
	} __attribute__ ((packed));
} pg_dir_entr_t;

/* Page table entry that points to page frames of size 4kB */
typedef union pg_tbl_entr_t {
	uint32_t val;
	struct {
		uint8_t  present    :  1;  /* 1-bit present flag               */
		uint8_t  rw         :  1;  /* 1-bit read/write flag            */
		uint8_t  privilege  :  1;  /* 1-bit, user/supervisor           */
		uint8_t  pwt        :  1;  /* 1-bit write through flag         */
		uint8_t  pcd        :  1;  /* 1-bit cache disable flag         */
		uint8_t  accessed   :  1;  /* 1-bit accessed flag              */
		uint8_t  dirty      :  1;  /* 1-bit dirty flag                 */
		uint8_t  pat        :  1;  /* 1-bit page attribute table flag  */
		uint8_t  global     :  1;  /* 1-bit global flag                */
		uint8_t  avl        :  3;  /* 3-bit available flag             */
		uint32_t addr       : 20;  /* 20-bit page frame indxe          */
	} __attribute__ ((packed));
} pg_tbl_entr_t;

/* Page directory */
typedef pg_dir_entr_t pg_dir_t[NUM_PG_DIR_ENTRY];
/* Page table */
typedef pg_tbl_entr_t pg_tbl_t[NUM_PG_TBL_ENTRY];

extern pg_dir_t page_directory;
extern pg_tbl_t page_table;

/* Initialize paging */
void page_init(void);
/* Set paging for user program */
void set_user_prog_page(uint32_t pid);
/* Set video mapping for user program */
void set_vidmap_page(uint8_t** screen_start);

#endif
