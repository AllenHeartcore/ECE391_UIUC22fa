#include "lib.h"
#include "x86_desc.h"
#include "filesys.h"
#include "syscall.h"


boot_block_t* boot_block; // Pointer to the boot block
inode_t* inodes;  // Pointer to the inodes
data_block_t* data_blocks;  // Pointer to the data blocks
dentry_t* dentries; // Pointer to the dentries
int index_file = 0;

datablock_track_t datablock_track; // Tracks if a datablock is free

/*
*   file_system_init
*   Initialize the file system
*   input: uint32_t file_add -- the address of file img
*   output: None
*   side effect: boot_block, inodes, data_blocks, dentries will be changed
*/
void file_system_init(uint32_t file_add){
    uint32_t i, j;
    uint32_t num_datablocks;

    boot_block = (boot_block_t*)file_add; // Find boot block

    inodes = (inode_t*)(boot_block + 1);  // Find inodes

    data_blocks = (data_block_t*)(inodes + boot_block->inode_num);  // Find data blocks

    dentries = boot_block->dentries;  // Find dentries

    /* Track datablocks */
    datablock_track.num_datablocks = 0;
    memset(datablock_track.used, 0, 64);
    for (i = 0; i < boot_block->inode_num; i++) {
        num_datablocks = (inodes[i].len + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for (j = 0; j < num_datablocks; j++) {
            datablock_track.used[inodes[i].data_block[j]] = 1;
        }
        datablock_track.num_datablocks += num_datablocks;
    }
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
    if (fname == NULL)  
        return -1;
    if (strlen((int8_t*)fname) > 32)
        return -1;
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

    length = (length + offset > inodes[inode_index].len) ?
                inodes[inode_index].len - offset : length;
    if(length == 0)
        return 0;

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

/*
 * RETURNS: -1 on failure, data block index on success
 * SIDE EFFECT: offset is set to BLOCK_SIZE if success
 * */
int32_t _find_empty_datablock(int32_t* offset) {
	int i;
	for (i = 0; i < datablock_track.num_datablocks; i++) {
		if (!datablock_track.used[i]) {
			*offset = BLOCK_SIZE;
			return i;
		}
	}
	return -1;
}

/*
 * RETURNS: -1 on failure, data block index on success
 * SIDE EFFECT: offset is set to the offset of the file where
 *				the data block should be written if success
 * */
int32_t _find_available_datablock_for(uint32_t target_inode_index, int32_t* offset) {
	uint32_t target_num_datablocks;
	uint32_t target_offset;
	uint32_t last_datablock_index;
	data_block_t *target_datablock = NULL;

	/* Sanity check */
	if (target_inode_index > (boot_block->inode_num - 1)) {
		return -1;
	}

	target_offset = inodes[target_inode_index].len % BLOCK_SIZE;
	target_num_datablocks = (inodes[target_inode_index].len + BLOCK_SIZE - 1) / BLOCK_SIZE;
	last_datablock_index = inodes[target_inode_index].data_block[target_num_datablocks - 1];

	if (target_offset == 0) {
		return _find_empty_datablock(offset);
	}

	*offset = target_offset;
	return last_datablock_index;
}

/* write_data
 * Writes data to the filesystem.
 * INPUT:	uint32_t inode_index -- the index of inode to write into
 * 			uint8_t* buf -- buffer containing data to be written
 * 			uint32_t length -- length of the buffer
 * RETURNS:	bytes of data we have successfully written
 * SIDE EFFECT: change the file system
 */
int32_t write_data(uint32_t inode_index, const uint8_t* buf, uint32_t length){
	uint32_t i;
	int32_t offset;
	uint32_t start_block_index;	// Which data block we should start to writing
	uint32_t start_pos_inblock;	// Which position in the start block we should start to write
	uint32_t end_block_index;	// Which data block we should end writing
	uint32_t end_pos_inblock;	// Which position in the last block we should end writing
	uint32_t bytes_written = 0;
	uint32_t bytes_to_write;
	int32_t  cur_datablock_index;	// The index of current data block in data_blocks array

	printf("write_data: inode_index = %d, length = %d, buf = %s\n",	// DEBUG
		inode_index, length, buf);		// DEBUG
	/* Sanity check */
	if(inode_index > (boot_block->inode_num - 1) || buf == NULL)
		return 0;

	while (bytes_written < length) {
		cur_datablock_index = _find_available_datablock_for(inode_index, &offset);
		printf("contents of current data block:%s\n",		// DEBUG
			(data_blocks[cur_datablock_index].data));		// DEBUG
		printf("cur_datablock_index = %d, offset = %d\n",	// DEBGU
			cur_datablock_index, offset);					// DEBUG
		/* No available data block found */
		if (cur_datablock_index == -1) {
			return 0;
		}
		bytes_to_write = BLOCK_SIZE - offset > length - bytes_written ?
				length - bytes_written : BLOCK_SIZE - offset;
		printf("byte to write = %d\n", bytes_to_write);		//DEBUG
		memcpy(&((data_blocks[cur_datablock_index].data)[offset]),
			&(buf[bytes_written]), bytes_to_write);

		/* Update relevant info */
		bytes_written += bytes_to_write;
		printf("data block %d before writing is %s\n", cur_datablock_index,				// DEBUG
			datablock_track.used[cur_datablock_index] == 1 ? "used" : "not used");		// DEBUG
		datablock_track.used[cur_datablock_index] = 1;
		printf("data block %d after writing is %s\n", cur_datablock_index,				// DEBUG
			datablock_track.used[cur_datablock_index] == 1 ? "used" : "not used");		// DEBUG
		printf("file length before writing is %d\n", inodes[inode_index].len);			// DEBUG
		inodes[inode_index].len += bytes_to_write;
		printf("file length after writing is %d\n", inodes[inode_index].len);			// DEBUG
		/* offset == 0 indicates we are writing to a new data block for
		 * current inode thus updating the data_block array is needed */
		if (offset == 0) {
			inodes[inode_index].data_block[(inodes[inode_index].len
				+ BLOCK_SIZE - 1) / BLOCK_SIZE - 1] = cur_datablock_index;
		}
	}
	printf("total bytes written = %d\n", bytes_written);			// DEBUG
	printf("contents of current data block after writing:\n%s\n",	// DEBUG
		(data_blocks[cur_datablock_index].data));					// DEBUG
	return bytes_written;
}

/* Read all the descriptions of the files
 * 
 * Asserts that we can read data from data block
 * Inputs: buffer to store the file description data
 * Outputs: Return filename's length
 * Side Effects: None
 * Coverage: Load all the filename in the directory
 */
int32_t read_directory(uint8_t* buf, int index){
    dentry_t dentry;                // initialize the dentry
    uint32_t string_length;
    // get the certain dentry
    dentry = dentries[index];
    if (index >= boot_block->dir_num)
        return -1;
    // copy the filename into buffer
    memcpy(buf, &dentry.filename, FILE_NAME_MAX);

    string_length = strlen((int8_t*)dentry.filename);
    return string_length < FILE_NAME_MAX ? string_length : FILE_NAME_MAX;
}


/*
 * fopen
 * check whether we can open the file with filename
 * Inputs: fname
 * Output: 0 if success -1 if fail
 * Side Effect: None
 * Coverage: Check whether we can open the file
 */
int32_t fopen(const uint8_t* filename){
    dentry_t dentry;
    // call the function read_dentry_by_name
    if (read_dentry_by_name(filename, &dentry))
        return -1;
    else
        return 0;
}


/*
 * fopen
 * check whether we can close the file with filename
 * Inputs: fd (file descriptor)
 * Output: 0 if success -1 if fail
 * Side Effect: None
 * Coverage: Check whether we can close the file
 */
int32_t fclose(int32_t fd){
    return 0;
}

/*
 * fwrite
 * write into file
 * Input: fd: file descriptor, buffer: written data , n_bytes: how many bytes written
 * Output: number of bytes written if success, -1 if failure
 * Side Effect: File maybe changed
 * Coverage: Write into files
 */
int32_t fwrite(int32_t fd, const void* buf, int32_t n_bytes){
    /* Sanity check */
    if (fd < 0 || fd > 7 || buf == NULL || n_bytes < 0) {
        return -1;
    }

    pcb_t* cur_pcb = get_cur_pcb();
    file_desc_t* file_desc = cur_pcb->file_descs;
    uint32_t inode = file_desc[fd].inode;
    uint32_t offset = file_desc[fd].file_position;
    if (write_data(inode, buf, n_bytes) == n_bytes) {
        return n_bytes;
    }
    return -1;
}

/*
 * fread
 * read file
 * Input: fd: file descriptor, buffer: written data , n_bytes: how many bytes written
 * Output: 0 if success -1 if failure
 * Side Effect: None
 * Coverage: Read the files
 */
int32_t fread(int32_t fd, void* buf, int32_t n_bytes){
    pcb_t* cur_pcb = get_cur_pcb();
    file_desc_t* file_desc = cur_pcb->file_descs;
    uint32_t inode = file_desc[fd].inode;
    uint32_t offset = file_desc[fd].file_position;
    return read_data(inode, offset, buf, n_bytes);
}

/*
 * dir_open
 * check whether we can open the directory with filename
 * Inputs: directory name
 * Output: 0 
 * Side Effect: None
 * Coverage: Check whether we can open the file
 */
int32_t dir_open(const uint8_t* filename){
    return 0;
}

/*
 * dir_read
 * read directory
 * Input: fd: file descriptor, buffer: written data , n_bytes: how many bytes written
 * Output: 0 if success -1 if failure
 * Side Effect: None
 * Coverage: Read the files
 */
int32_t dir_read(int32_t fd, void* buf, int32_t n_bytes){
    int ret_val;
	// 63 is the max directory number in filesystem
	ret_val = read_directory(buf, index_file);
    index_file++;
    if (ret_val == -1){
        index_file = 0;
        return 0;
    }
    return ret_val;
}

/*
 * dir_write
 * write directory
 * Input: fd: file descriptor, buffer: written data , n_bytes: how many bytes written
 * Output: 0 if success -1 if failure
 * Side Effect: None
 * Coverage: Read the files
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t n_bytes){
    return 0;
}

/*
 * dir_open
 * check whether we can close the directory with filename
 * Inputs: fd (file descriptor)
 * Output: 0 if success -1 if fail
 * Side Effect: None
 * Coverage: Check whether we can close the file
 */
int32_t dir_close(int32_t fd){
    return 0;
}




