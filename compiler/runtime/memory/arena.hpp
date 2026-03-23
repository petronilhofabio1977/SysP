#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>

struct Arena {

    uint8_t* memory;
    size_t capacity;
    size_t offset;

};

inline void arena_init(Arena* arena, size_t size)
{
    arena->memory = (uint8_t*)malloc(size);
    arena->capacity = size;
    arena->offset = 0;
}

inline void* arena_alloc(Arena* arena, size_t size)
{
    void* ptr = arena->memory + arena->offset;
    arena->offset += size;
    return ptr;
}

inline void arena_reset(Arena* arena)
{
    arena->offset = 0;
}
