#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"
#include "syscall.h"
#include "page.h"

/* 
ZichengMa  2022.11.26
In this specific implementation, fixed length memory allocation is implemented by two parts, memory management unit and memory unit.
fixed length diagram:
        |memory management unit|  |memory management unit|  |memory management unit|  ..........    |memory unit|  |memory unit| |memory unit| .....
        |next|  --------------->  |next|  --------------->  |next|                                      ↑              ↑             ↑
        |ptr|---------------------|ptr|---------------------|ptr|---------------------------------------↑--------------↑-------------↑
On the other hand, variable length memory allocation is implemented very similar to Linux's implicit free list design. 
A memory part consists of two parts, memory management unit and real useable memory unit.
Memory management units are held in a linked list to track each memory fragments have been allocated.
variable length diagram:
        |memory mangement unit| -> |memory mangement unit| -> |memory mangement unit| -> |memory mangement unit|
        -----------------------    -----------------------    -----------------------    -----------------------
        |                     |    |                     |    |                     |    |                     |
        |   useable memory    |    |   useable memory    |    |   useable memory    |    |   useable memory    |
        |                     |    |                     |    -----------------------    |                     |
        |                     |    -----------------------                               -----------------------
        |                     |
        -----------------------

Slab cache's implementation is based on fixed length memory allocation.
 */








#define UNIT_FIXED_LEN_SIZE 32 // Each memory unit in fixed length memory allocation is 32 bytes
#define NODE_SIZE sizeof(fmem_node) // Size of a memory management node in linked list
#define FIX_LEN_MEMORY_START 160*1024*1024  // 160MB
#define SLAB_SIZE 4*1024      // A slab's zie is 4kb = a page size
#define FIX_LEN_MEMORY_MAX_SIZE 4*1024*1024 // 4MB
#define VAR_LEN_MEMORY_START 164*1024*1024  // 164MB
#define VAR_LEN_MEMORY_MAX_SIZE 4*1024*1024 // 4MB


/* Memory management structure node (node of a linked list) */
typedef struct  fmem_node {
    struct fmem_node* next;
    void* ptr;
}  fmem_node;

/* Linked list storing memory's information */
typedef struct  fmem_list {
    fmem_node* head;    // Head of the linked list
    fmem_node* node_base; // Start address of memory management struct
    void* unit_base;     // Start address of useable memory
    uint32_t  max_units;  // Max number of units in a slab can have
    uint32_t  used_units;
    uint32_t  size;   // Structure's size
    struct fmem_list* next;
}  fmem_list;

/* Definition of a slab cache */
typedef struct  slab_cache {
    uint8_t   name[20+1];  // Name has at most 20 chars, 1 for EOS
    fmem_list* slabs; // Each cache use at most 10 slabs
    uint32_t size; // Structure's size in this slab cache
}  slab_cache;



/* Memory management structure node (node of a linked list) */
typedef struct  vmem_node {
    struct vmem_node* next;
    struct vmem_node* prev;
    uint8_t* ptr;
    uint32_t  used;
    uint32_t  free;
}  vmem_node;

/* Fixed length functions */
void fmem_init(uint32_t mem, uint32_t size, fmem_list* memlist);
void* malloc_fixlen(fmem_list* memlist);
int32_t free_fixlen(fmem_list* memlist, void* ptr);
/* Variable length functions */
void vmem_init(uint32_t mem, uint32_t size);
void* malloc_varlen(uint32_t size);
int32_t free_varlen(void* ptr);
/* Slab cache's functions */
void slab_cache_init();
slab_cache* slab_cache_create(const char* name, uint32_t size);
void* slab_cache_alloc(slab_cache* cache);
int32_t slab_cache_free(slab_cache* cache, void* ptr);
void slab_cache_destroy(slab_cache* cache);

/* Static variables */
extern vmem_node* vmem_head;
extern fmem_list slab_cache_list;
extern fmem_list slabs_list;


/* Functions for visualize */
extern void visual_slab_caches();
extern void visual_varmem();

#endif

