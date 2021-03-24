#include "mem.h"

void stack_create(stack_t *self, void *buffer, size_t size)
{
    self->base = (uptr) buffer;
    self->max = self->base + (uptr) size;
    self->pos = self->max;
}

void *stack_push_alloc(stack_t *self, u32 size)
{
    size = size + (HEX_MEMORY_ALIGNMENT - (size % HEX_MEMORY_ALIGNMENT));

    uptr alloc_pos = self->pos - size;
    uptr is_valid = (uptr) (alloc_pos >= self->base);

    self->pos -= is_valid * (size + sizeof(u32));
    *((u32*) self->pos) = is_valid * (size + sizeof(u32)) + (is_valid ^ 1) * *((u32*) self->pos);

    return (void*) (is_valid * alloc_pos);
}

void stack_pop_alloc(stack_t *self)
{
    uptr delta = (uptr) (self->pos != self->max) * *((u32*) self->pos);
    self->pos += delta;
}

void stack_flush(stack_t *self)
{
    self->pos = self->max;
}



// memory arena

void arena_create(memory_arena_t *self, void *buffer, size_t size)
{
    self->base = (uptr) buffer;
    self->max = self->base + (uptr) size;
    self->pos = self->max;
}

void *arena_alloc(memory_arena_t *self, size_t size)
{
    size = size + (HEX_MEMORY_ALIGNMENT - (size % HEX_MEMORY_ALIGNMENT));
    uptr is_valid = (uptr) ((self->pos - size) >= self->base);
    self->pos -= is_valid * size;
    return (void*) (is_valid * self->pos);
}

void arena_flush(memory_arena_t *self)
{
    self->pos = self->max;
}




// global memory

static void *memory_base;
memory_arena_t perm_mem;
stack_t temp_mem;

void memory_create(size_t perm_size, size_t temp_size)
{
    perm_size = perm_size + (HEX_MEMORY_ALIGNMENT - (perm_size % HEX_MEMORY_ALIGNMENT));
    temp_size = temp_size + (HEX_MEMORY_ALIGNMENT - (temp_size % HEX_MEMORY_ALIGNMENT));
    
    memory_base = malloc(perm_size + temp_size);
    uptr memory = (uptr) memory_base;
    
    arena_create(&perm_mem, (void*) memory, perm_size);
    memory += perm_size;
    stack_create(&temp_mem, (void*) memory, temp_size);
    memory += temp_size;
}

void memory_destroy()
{
    free(memory_base);
}