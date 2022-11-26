#include "memory.h"

static mem_list memlist;

void mem_init(uint32_t mem, uint32_t size){
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
    memlist.node_base = (mem_node*)mem;
    memlist.unit_base = (mem_unit*)(mem + max*NODE_SIZE);
    memlist.head = (mem_node*)mem;

    mem_node* cur_node;
    for(i=0; i<max-1; i++){
        cur_node = memlist.head + i;
        cur_node->next = memlist.head + i + 1;  // Make each next ptr pointing to next node

    }
    cur_node = memlist.head + max;
    cur_node->next = NULL; // The last node points to NULL
}

void* malloc_fixlen(){
    void* ret = NULL;
    if(memlist.head != NULL){
        mem_node* alloc = memlist.head; // The memory will be allocated will be the head of the linked list
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
        uint32_t index = ((uint32_t)ptr - (uint32_t)memlist.unit_base) / sizeof(mem_unit);
        mem_node* free_node = memlist.node_base + index;
        if(index < memlist.max_units && free_node->ptr == ptr){
            /* We can free a chunk of memory only if it is assigned by us */

            /* Insert this mem_node in the front of the linked list */
            free_node->next = memlist.head;
            memlist.head = free_node;
            ret = 1;
        }
    }
    return ret;
}


