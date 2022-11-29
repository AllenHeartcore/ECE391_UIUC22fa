#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"
#include "memory.h"
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
#define FIX_LEN_MEMORY_MAX_SIZE 4*1024*1024 // 4MB
#define VAR_LEN_MEMORY_START 164*1024*1024  // 164MB
#define VAR_LEN_MEMORY_MAX_SIZE 4*1024*1024 // 4MB

void fmem_init(uint32_t mem, uint32_t size);
void* malloc_fixlen();
int32_t free_fixlen(void* ptr);


void vmem_init(uint32_t mem, uint32_t size);
void* malloc_varlen(uint32_t size);
int32_t free_varlen(void* ptr);
 
/* Memory unit for fixed length memory allocation */
typedef struct  fmem_unit {
    uint8_t data[UNIT_FIXED_LEN_SIZE];
}  fmem_unit;

/* Memory management structure node (node of a linked list) */
typedef struct  fmem_node {
    struct fmem_node* next;
    fmem_unit* ptr;
}  fmem_node;

/* Linked list storing memory's information */
typedef struct  fmem_list {
    fmem_node* head;
    fmem_node* node_base;
    fmem_unit* unit_base;
    uint32_t  max_units;
}  fmem_list;

/* Definition of a slab cache */
typedef struct  slab_cache {
    fmem_node* head;
    fmem_node* node_base;
    fmem_unit* unit_base;
    uint32_t  max_units;
}  slab_cache;



/* Memory management structure node (node of a linked list) */
typedef struct  vmem_node {
    struct vmem_node* next;
    struct vmem_node* prev;
    uint8_t* ptr;
    uint32_t  used;
    uint32_t  free;
}  vmem_node;



#endif

