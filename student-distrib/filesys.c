#include "lib.h"
#include "x86_desc.h"
#include "filesys.h"



boot_block_t* boot_block; // Pointer to the boot block
inode_t* inodes;  // Pointer to the inodes
data_block_t* data_blocks;  // Pointer to the data blocks
dentry_t* dentries; // Pointer to the dentries

/*
*   file_system_init
*   Initialize the file system
*   input: uint32_t file_add -- the address of file img
*   output: None
*   side effect: boot_block, inodes, data_blocks, dentries will be changed
*/
void file_system_init(uint32_t file_add){

    boot_block = (boot_block_t*)file_add; // Find boot block

    inodes = (inode_t*)(boot_block + 1);  // Find inodes

    data_blocks = (data_block_t*)(inodes + boot_block->inode_num);  // Find data blocks

    dentries = boot_block->dentries;  // Find dentries
}



/*
*   read_dentry_by_index
*   Read the content of a dentry with given index
*   input: uint8_t index -- the index of dentry we want
*          dentry_t* dentry -- we will copy value to this ptr
*   output: -1 for failure and 0 for success
*   side effect: the data pointed by dentry may be changed
*/
int32_t read_dentry_by_index (uint8_t index, dentry_t* dentry){
    /* Sanity check */
    if(index >= boot_block->dir_num || index > BOOT_BLOCK_MAX_INDEX ){
        return -1;
    }
    dentry_t* dentry_ptr = &(dentries[index]);
    /* copy two integer */
    dentry->filetype = dentry_ptr->filetype;
    dentry->inode_num = dentry_ptr->inode_num;
    /* copy file name
    *  Here, we must use strncpy because file name can have no \0 */
    strncpy((int8_t*)dentry->filename, (int8_t*)dentry_ptr->filename, FILE_NAME_MAX);
    return 0;
}

/*
*   read_dentry_by_name
*   Read the content of a dentry with given index
*   input: const uint8_t* fname -- the name of dentry we want
*          dentry_t* dentry -- we will copy value to this ptr
*   output: -1 for failure and 0 for success
*   side effect: the data pointed by dentry may be changed
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    uint8_t i;
    dentry_t* current_dentry;
    for(i=0; i<boot_block->dir_num; i++){
        current_dentry = &(dentries[i]);
        if(strncmp((int8_t*)fname, (int8_t*)current_dentry->filename, FILE_NAME_MAX)==0){
            /* The file name is the same, use read_index to assign value */
            read_dentry_by_index(i, dentry);
            return 0;
        }
    }
    return -1;

}



/*
*   read_data
*   Read the content of a dentry with given index
*   input: uint32_t inode_index -- the index of inode we want
*          uint32_t offset -- which position of the file we want to start of our reading process
*          uint8_t* buf -- buffer for the data read from file
*          uint32_t length -- how many bytes we want to read
*   output: bytes of data we have successfully read
*   side effect: the data in buffer may be changed
*/
int32_t read_data(uint32_t inode_index, uint32_t offset, uint8_t* buf, uint32_t length){
    inode_t* inode;
    uint32_t start_block_index;  // Which data block we should start to read
    uint32_t start_pos_inblock;  // Which position in the start block we should start to read
    uint32_t end_block_index;  // Which data block we should end reading
    uint32_t end_pos_inblock;  // Which position in the last block we should end reading
    uint32_t i, bytes_copied;
    uint32_t    cur_datablock_index; // The index of current data block in data_blocks array
    data_block_t* cur_datablock;   // Pointer to current data block
    /* Sanity check */
    /* If inode_index is out of range, we can not read any bytes */
    if(inode_index > (boot_block->inode_num-1) )
        return 0;
    /* If offset is already bigger than the file's total length, we can not read any bytes */
    if(inodes[inode_index].len < offset)
        return 0;
    if(length == 0)
        return 0;
    if (length>(inodes[inode_index].len - offset))
    {
        length = inodes[inode_index].len - offset;
    }
    
    inode = &(inodes[inode_index]);
    bytes_copied = 0;

    /* Calculate how many data blocks are used and start/end positions */
    start_block_index = offset / BLOCK_SIZE;
    start_pos_inblock = offset % BLOCK_SIZE;
    end_block_index = (length-1) / BLOCK_SIZE  + start_block_index;
    end_pos_inblock = (length-1) % BLOCK_SIZE + start_pos_inblock ;

    for(i=start_block_index; i<=end_block_index; i++){
        cur_datablock_index = (inodes[inode_index].data_block)[i];
        cur_datablock = &(data_blocks[cur_datablock_index]);
        /* Copy the start data block */
        if(i == start_block_index){
            if(start_block_index==end_block_index){
                /* If start block is the same as end block, only copy part of the data block */
                memcpy(buf, &(cur_datablock->data[start_pos_inblock]), length);
                buf += length;
                bytes_copied += length;
            }else{
                /* else, copy all the data in start block */
                memcpy(buf, &(cur_datablock->data[start_pos_inblock]), BLOCK_SIZE-start_pos_inblock); // Copy all the data in block from start pos
                buf += BLOCK_SIZE-start_pos_inblock;
                bytes_copied += BLOCK_SIZE-start_pos_inblock;
            }
            continue;
        }

        /* Copy the end data block */
        if(i==end_block_index){
            memcpy(buf, cur_datablock->data, end_pos_inblock+1 ); // Copy end_pos + 1 bytes
            buf += end_pos_inblock+1;
            bytes_copied += end_pos_inblock+1;
            break;
        }

        /* Copy the data blocks in the middle */
        memcpy(buf, cur_datablock->data, BLOCK_SIZE); // 4096 bytes in this data block should be copied
        buf += BLOCK_SIZE;
        bytes_copied += BLOCK_SIZE;
    }

    return bytes_copied;
}
