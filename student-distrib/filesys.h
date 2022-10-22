#ifndef FILESYS_H
#define FILESYS_H

#include "types.h"


#define BLOCK_SIZE 4096  // Each block is 4096 bytes
#define DIR_SIZE 64 // Each directory entry in boot block is 64 bytes
#define BOOT_BLOCK_MAX_INDEX 63 // Boot block can hold 64 entries including statistic part at most. So the biggest index is 63
#define FILE_NAME_MAX 32 // Longest file name is 32 bytes


#define RTC_TYPE 0 // Integer representing RTC type
#define DIR_TYPE 1  // Integer representing directory type
#define FILE_TYPE 2 // Integer representing regular file type




/* Data Structures for file system */

typedef struct  dentry_t { //__attribute__((packed))
    uint8_t   filename[FILE_NAME_MAX];
    uint32_t filetype;
    uint32_t inode_num;
    /* There are 24 reserved bytes, so we need an array with 6 int32 (6*32/8=24) */
    uint32_t reserved[6];  
}   dentry_t;

typedef struct  boot_block_t {
    uint32_t dir_num;
    uint32_t inode_num;
    uint32_t data_blocks_num;
    /* There are 52 reserved bytes, so we need an array with 13 int32 (13*32/8=52) */
    uint32_t reserved[13];
    dentry_t dentries[BOOT_BLOCK_MAX_INDEX];
}   boot_block_t;

typedef struct  inode_t {
    uint32_t len;
    /* There are 1023 data_block indices in total */
    uint32_t data_block[(BLOCK_SIZE-4)/4];
}   inode_t;

typedef struct  datablock_t {
    uint8_t data[BLOCK_SIZE];
}   data_block_t;




/* Functions for file system */

/* Find a dentry with name and load the content into argument dentry ptr */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

/* Find a dentry with index and load the content into argument dentry ptr */
int32_t read_dentry_by_index (uint8_t index, dentry_t* dentry);

/* Read data from an inode spec ified by inode_index */
int32_t read_data(uint32_t inode_index, uint32_t offset, uint8_t* buf, uint32_t length);




void file_system_init(uint32_t file_add);


int32_t file_open(const uint8_t* fname);

int32_t file_close(int32_t fd);

/* function we care */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);

int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t dir_open(const uint8_t* fname);

int32_t dir_close(int32_t fd);

int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t read_directory(uint8_t* buf, int index);
/* function we care */
int32_t dir_read();



#endif
