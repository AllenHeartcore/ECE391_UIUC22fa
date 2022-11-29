#include "memory.h"
#include "lib.h"
static fmem_list memlist;
static vmem_node* vmem_head;

void fmem_init(uint32_t mem, uint32_t size){
    uint32_t max;
    /* Calculate max units we can have */
    max = size /( NODE_SIZE + UNIT_FIXED_LEN_SIZE); 

    /* Set up page directory */
    uint32_t pages,i;
    pages = size / FOUR_MB; // How many 4MB pages are needed
    for(i=0; i<pages; i++){
        /* Initialize some 4MB pages */
        SET_PDE(page_directory, (mem >> 22) + i, 0, 1, 1, (mem >> 12) + i);
    }

    /* Initialize linked list */
    memlist.max_units = max;
    memlist.node_base = (fmem_node*)mem;
    memlist.unit_base = (fmem_unit*)(mem + max*NODE_SIZE);
    memlist.head = (fmem_node*)mem;

    fmem_node* cur_node;
    for(i=0; i<max-1; i++){
        cur_node = memlist.head + i;
        cur_node->next = memlist.head + i + 1;  // Make each next ptr pointing to next node

    }
    cur_node = memlist.head + max;
    cur_node->next = NULL; // The last node points to NULL
}

void vmem_init(uint32_t mem, uint32_t size){
    /* Set up page directory */
    uint32_t pages,i;
    pages = size / FOUR_MB; // How many 4MB pages are needed
    for(i=0; i<pages; i++){
        /* Initialize some 4MB pages */
        SET_PDE(page_directory, (mem >> 22) + i, 0, 1, 1, (mem >> 12) + i);
    }

    vmem_head = (vmem_node*)mem;
    vmem_head->next = vmem_head;
    vmem_head->prev = vmem_head;
    vmem_head->used = 0;
    vmem_head->free = size - sizeof(vmem_node); // Free space = total size - size of management node
    vmem_head->ptr = (uint8_t*)(mem + sizeof(vmem_node));  //  Start add = mem + the pace owned by vmem_node

}

void* malloc_fixlen(){
    void* ret = NULL;
    if(memlist.head != NULL){
        fmem_node* alloc = memlist.head; // The memory will be allocated will be the head of the linked list
        int index = ((uint32_t)alloc - (uint32_t)memlist.node_base) / NODE_SIZE; // This node's index
        ret = (void*)(memlist.unit_base + index);
        alloc->ptr = ret; // Store the allocated memory address. It will be used later in free process.

        /* Update head of the linked list */
        memlist.head = alloc->next;
    }
    return ret;
}


int32_t free_fixlen(void* ptr){
    int32_t ret = 0;
    if( ptr != NULL){
        uint32_t index = ((uint32_t)ptr - (uint32_t)memlist.unit_base) / sizeof(fmem_unit);
        fmem_node* free_node = memlist.node_base + index;
        if(index < memlist.max_units && free_node->ptr == ptr){
            /* We can free a chunk of memory only if it is assigned by us */

            /* Insert this fmem_node in the front of the linked list */
            free_node->next = memlist.head;
            memlist.head = free_node;
            ret = 1;
        }
    }
    return ret;
}


void* malloc_varlen(uint32_t size){
    vmem_node* cur_node = vmem_head;
    vmem_node* ret = NULL;
    uint32_t alloc_size = size + sizeof(vmem_node);
    while(1){
        if( cur_node->free >= alloc_size ){
            /* If we have enough space, calculate where should we split current memory */
            uint8_t* mem = (uint8_t*)( (uint32_t)cur_node->ptr + cur_node->used + cur_node->free - alloc_size );
            /* Modify new node */
            ret = (vmem_node*)mem;
            ret->used = size;
            ret->free = 0;
            ret->ptr = mem + sizeof(vmem_node);
            /* Modify old node */
            cur_node->free -= alloc_size;

            /* Add new node into linked list after the current one */
            ret->next = cur_node->next;
            cur_node->next = ret;
            ret->next->prev = ret;
            ret->prev = cur_node;
            break;
        }
        cur_node = cur_node->next;
        if(cur_node == vmem_head)
            break;
    } 

    return ret? ret->ptr:NULL;
}

int32_t free_varlen(void* ptr){
    int32_t ret = 0;
    vmem_node* cur_node = vmem_head;
    if(ptr != NULL){
        while(1){
            if((uint32_t)cur_node->ptr == (uint32_t)ptr){
                /* Update previous node */
                vmem_node* prev_node = cur_node->prev;
                prev_node->free += cur_node->used + cur_node->free + sizeof(vmem_node);
                /* Delete cur_node in linked list */
                prev_node->next = cur_node->next;
                prev_node->next->prev = prev_node;

                ret = 1;
                break;
            }
            cur_node = cur_node -> next;
            if(cur_node == vmem_head)
                break;
        }
    }
        
    return ret;
}