#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"
#include "memory.h"
#include "syscall.h"
#include "page.h"

#define UNIT_FIXED_LEN_SIZE 32 // Each memory unit in fixed length memory allocation is 32 bytes
#define NODE_SIZE sizeof(mem_node) // Size of a memory management node in linked list
#define FIX_LEN_MEMORY_START 160*1024*1024  // 160MB
#define FIX_LEN_MEMORY_MAX_SIZE 4*1024*1024 // 4MB

void mem_init(uint32_t mem, uint32_t size);
void* malloc_fixlen();
int32_t free_fixlen(void* ptr);

/* Memory unit for fixed length memory allocation */
typedef struct  mem_unit {
    uint8_t data[UNIT_FIXED_LEN_SIZE];
}  mem_unit;

/* Memory management structure node (node of a linked list) */
typedef struct  mem_node {
    struct mem_node* next;
    mem_unit* ptr;
}  mem_node;

/* Linked list storing memory's information */
typedef struct  mem_list {
    mem_node* head;
    mem_node* node_base;
    mem_unit* unit_base;
    uint32_t  max_units;
}  mem_list;


#endif

