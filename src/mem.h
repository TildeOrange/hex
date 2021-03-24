// @note I use 32-bit sizes for allocations, because I cannot see myself allocating more than 4gb (really no more than 100mb)
// however, for total memory sizes I use size_t for the future if the *TOTAL* memory used gets more than 4gb 

// @warning *NEVER* assume a new allocation is zeroed

#ifndef _MEM_H
#define _MEM_H

#include <stdlib.h> // size_t
#include "util.h"

#define KB(x) (((size_t) x) << 10)
#define MB(x) (((size_t) x) << 20)
#define GB(x) (((size_t) x) << 30)

#define HEX_MEMORY_ALIGNMENT sizeof(void*)

typedef struct stack_t
{
    uptr base, pos, max;
} stack_t;

void stack_create(stack_t *self, void *buffer, size_t size);
void *stack_push_alloc(stack_t *self, u32 size);
void stack_pop_alloc(stack_t *self);
void stack_flush(stack_t *self);

typedef struct memory_arena_t
{
    uptr base, pos, max;
} memory_arena_t;

void arena_create(memory_arena_t *self, void *buffer, size_t size);
void *arena_alloc(memory_arena_t *self, size_t size);
void arena_flush(memory_arena_t *self);

void memory_create(size_t perm_size, size_t temp_size);
void memory_destroy();

// global memory structs
extern memory_arena_t perm_mem;
extern stack_t temp_mem;

#define perm_alloc(sz) arena_alloc(&perm_mem, sz)
#define temp_alloc(sz) stack_push_alloc(&temp_mem, sz)
#define temp_free(ptr) stack_pop_alloc(&temp_mem) // ptr is for future use also it helps you keep track

#endif