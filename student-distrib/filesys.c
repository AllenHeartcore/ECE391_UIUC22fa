#include "lib.h"
#include "x86_desc.h"
#include "filesys.h"





/* Find a dentry with name and load the content into argument dentry ptr */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){


}

/* Find a dentry with index and load the content into argument dentry ptr */
int32_t read_dentry_by_index (uint8_t index, dentry_t* dentry){

    
}

/* Read data from an inode spec ified by inode_index */
int32_t read_data(uint32_t inode_index, uint32_t offset, uint8_t* buf, uint32_t length){
    

}