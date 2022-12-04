#include "memory.h"
#include "lib.h"
// static fmem_list memlist;
vmem_node* vmem_head;
fmem_list slab_cache_list;
fmem_list slabs_list;
/* 
    mem: start address of new slab
    size: structure's size
 */
void fmem_init(uint32_t mem, uint32_t size, fmem_list* memlist){
    uint32_t max,i;
    /* Calculate max units we can have */
    max = SLAB_SIZE /( NODE_SIZE + size ); 

    /* Initialize linked list */
    memlist->max_units = max;
    memlist->used_units = 0;
    memlist->node_base = (fmem_node*)mem;
    memlist->unit_base = (void*)(mem + max*NODE_SIZE);
    memlist->head = (fmem_node*)mem;

    fmem_node* cur_node;
    for(i=0; i<max-1; i++){
        cur_node = memlist->head + i;
        cur_node->next = memlist->head + i + 1;  // Make each next ptr pointing to next node
        cur_node->ptr= 0;
    }
    cur_node = memlist->head + max;
    cur_node->next = NULL; // The last node points to NULL

    memlist->next = NULL;
    memlist->size = size;
}

void* malloc_fixlen(fmem_list* memlist){
    void* ret = NULL;
    if(memlist->head != NULL){
        fmem_node* alloc = memlist->head; // The memory will be allocated will be the head of the linked list
        int index = ((uint32_t)alloc - (uint32_t)memlist->node_base) / NODE_SIZE; // This node's index
        ret = (void*)(memlist->unit_base + index*(memlist->size));
        alloc->ptr = ret; // Store the allocated memory address. It will be used later in free process.

        /* Update head of the linked list */
        memlist->head = alloc->next;
        memlist->used_units +=1;
    }
    return ret;
}

/* 
    memlist -- a pointer to a specific
    ptr -- the pointer we want to free
    size -- size of the structure in current slab cache
 */
int32_t free_fixlen(fmem_list* memlist, void* ptr){
    int32_t ret = 0;
    if( ptr != NULL){
        uint32_t index = ((uint32_t)ptr - (uint32_t)memlist->unit_base) / memlist->size;
        fmem_node* free_node = memlist->node_base + index;
        if(index < memlist->max_units && free_node->ptr == ptr){
            /* We can free a chunk of memory only if it is assigned by us */
            /* Insert this fmem_node in the front of the linked list */
            free_node->next = memlist->head;
            memlist->head = free_node;
            ret = 1;
            memlist->used_units -=1;
            free_node->ptr = NULL;
        }
    }
    return ret;
}



void slab_cache_init(){
    fmem_init(FIX_LEN_MEMORY_START, sizeof(slab_cache), &slab_cache_list);
    fmem_init(FIX_LEN_MEMORY_START + SLAB_SIZE, sizeof(fmem_list), &slabs_list);
}

slab_cache* slab_cache_create(const char* name, uint32_t size){
    uint32_t i;
    slab_cache* ret = NULL;
    /* Sanity check */
    if(name == NULL || size >= (SLAB_SIZE - NODE_SIZE) )
        return ret;
    /* Find a free page */
    for(i = 2; i < NUM_PG_DIR_ENTRY; i++){
        if(slab_page_table[i].present == 0){
            /* Set the free page as present */
            slab_page_table[i].present = 1;
            break;
        }
    }
    if(i == NUM_PG_DIR_ENTRY)
        return ret;
    /* Allocate space from slab_cache page and slab_list page */
    ret = (slab_cache*)malloc_fixlen(&slab_cache_list);
    ret->slabs = (fmem_list*)malloc_fixlen(&slabs_list);
    ret->size = size;
    strncpy((int8_t*)ret->name, (int8_t*)name, 20);
    ret->name[20] = 0;
    /* Assign a new 4kb space for this cache */
    fmem_init(i*SLAB_SIZE + FIX_LEN_MEMORY_START, size, ret->slabs);

    return ret;
}

void* slab_cache_alloc(slab_cache* cache){
    void* ret = NULL;
    /* Sanity check */
    if(cache == NULL)
        return ret;
    /* Malloc space from this slab cache */
    fmem_list* cur_slab = cache->slabs;
    while(1){
        ret = malloc_fixlen(cur_slab);
        if(ret == NULL){
            /* If ret == NULL, current slab has no free space. Check next one */
            if(cur_slab->next == NULL){
                /* No free slabs, slab cache will grow automatically */
                int32_t i;
                /* Find a free page */
                for(i = 2; i < NUM_PG_DIR_ENTRY; i++){
                    if(slab_page_table[i].present == 0){
                        /* Set the free page as present */
                        slab_page_table[i].present = 1;
                        break;}}
                if(i == NUM_PG_DIR_ENTRY)
                    return ret;
                fmem_list* new_slab = (fmem_list*)malloc_fixlen(&slabs_list); // Get a new fmemelist struct
                fmem_init(i*SLAB_SIZE + FIX_LEN_MEMORY_START, cache->size, new_slab); // Get a new slab
                cur_slab->next = new_slab;
            }
            cur_slab = cur_slab->next;
            continue;
        }
        break;
    }
    return ret;
}

int32_t slab_cache_free(slab_cache* cache, void* ptr){
    int32_t ret = 0;
    /* Sanity check */
    if( cache == NULL || ptr == NULL)
        return ret;
    /* Find which slab owns this ptr */
    fmem_list* cur_slab = cache->slabs;
    fmem_list* prev_slab = cur_slab; // Used to shrink slab cache
    while(cur_slab != NULL){
        if((uint32_t)cur_slab->node_base + SLAB_SIZE > (uint32_t)ptr)
            break;
        prev_slab = cur_slab;
        cur_slab = cur_slab->next;  // Update current and previous slab ptr
    }
    if(cur_slab == NULL) // This ptr doesn't belong to this cache
        return ret; 
    /* Free the ptr */
    ret = free_fixlen(cur_slab, ptr);
    /* If current slab is empty, slab cache should shrink */
    if(cur_slab->used_units == 0){
        uint32_t page_index;
        page_index =  ((uint32_t)cur_slab->head - FIX_LEN_MEMORY_START) / (SLAB_SIZE);
        slab_page_table[page_index].present = 0;  // Free page
        prev_slab->next = cur_slab->next;   // Update linked list
        free_fixlen(&slabs_list, cur_slab); // Free current slab in the slabs_list
    }
    return ret;
}

void slab_cache_destroy(slab_cache* cache){
    /* Sanity check */
    if(cache == NULL)
        return;
    /* Check if there is any slabs are still used */
    fmem_list* cur_slab = cache->slabs;
    while(cur_slab != NULL){
        if(cur_slab->head != cur_slab->node_base){
            printf("The slab cache is not empty now, you can not destroy it!\n");
            return;
        }
        cur_slab = cur_slab->next;
    }
    /* Free slab structs in slabs_list */
    if( !free_fixlen(&slabs_list, (void*)cache->slabs))
        printf("Something goes wrong with your slab destroy!\n");
        return;
    /* Free cache itself */
    if( !free_fixlen(&slab_cache_list, (void*)cache))
        printf("Something goes wrong with your slab destroy!\n");
}





/* -------------------------Variable length memory allocation below----------------------- */
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

/* ------------------------Visualization--------------------- */
void visual_slab_caches(){
    /* Go through all the nodes in slab_cache_list and find those are used */
    fmem_node* cur_node;
    cur_node = slab_cache_list.node_base;
    int32_t i;
    printf("\n---------------Info of current slab caches---------------\n");
    for(i=0; i<slab_cache_list.max_units; i++){
        if(cur_node->ptr != 0){
            show_slab_cache((slab_cache*)cur_node->ptr);
        }
        cur_node = cur_node + 1;
    }
}

void visual_varmem(){
    vmem_node* cur_vnode;
    int32_t vnode_num;
    cur_vnode = vmem_head;
    vnode_num = 0;
    printf("\n-------------Info about variable length memory allocation------------\n");
    while(1){
        vnode_num +=1;
        printf("Variable length memory unit%d ---  free:%d  used:%d  address:%x\n", vnode_num, cur_vnode->free, cur_vnode->used, (uint32_t)cur_vnode->ptr);
        cur_vnode = cur_vnode->next;
        if(cur_vnode == vmem_head)
            break;
    }
    printf("--------------------------------------------------------------\n");
}

/* 
    input: slab_cache* slab
    return: none
    result: print some infomation about a specific slab
 */
void show_slab_cache(slab_cache* slab_cache){
    printf("Slab name: %s  slab_cache's ptr:%x ",slab_cache->name, (uint32_t)slab_cache);
    printf("size = %d \n", slab_cache->size);
    /* Go through all the slabs owned by this slab cache */
    int32_t slabs_num, used_unit_num,i;
    fmem_list* cur_slab;
    slabs_num = 0;
    used_unit_num = 0;
    cur_slab = slab_cache->slabs;
    while(cur_slab!=NULL){
        slabs_num +=1;
        fmem_node* temp_node;
        temp_node = cur_slab->node_base;
        for(i=0; i<cur_slab->max_units; i++){
            if(temp_node->ptr != 0){
                printf("ptr:%x  ",temp_node->ptr);
            }
            temp_node = temp_node + 1;
        }
        used_unit_num += cur_slab->used_units;
        cur_slab = cur_slab->next;
    }
    printf("\n%d slabs in total and  each slab has max %d units\n", slabs_num, slab_cache->slabs->max_units);
    printf("Until now, %d units have been allocated\n", used_unit_num);
    printf("---------------------------------------------------------\n"); 
}