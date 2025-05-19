#pragma once

#include "amber_forge.h"

#include <stddef.h>

typedef struct AFarena AFarena;
struct AFarena {
    unsigned char *buf;
    size_t buf_len;
    size_t prev_offset;
    size_t curr_offset;
};

AFAPI AF_INLINE bool is_power_of_two(uintptr_t x) { return (x & (x - 1)) == 0; }

AFAPI void arena_init(AFarena *a, void *backing_buffer, size_t backing_buffer_len);
AFAPI void *arena_alloc(AFarena *a, size_t len);
AFAPI void *arena_alloc_align(AFarena *a, size_t len, size_t align);
AFAPI void *arena_resize(AFarena *a, void *oldmem, size_t osize, size_t nsize);
AFAPI void *arena_resize_align(AFarena *a, void *oldmem, size_t osize, size_t nsize, size_t align);
AFAPI void arena_free(AFarena *a, void *ptr);
AFAPI void arena_free_all(AFarena *a);

#define afArenaInit(a, backing_buffer, backing_buffer_len) arena_init(a, backing_buffer, backing_buffer_len)
#define afArenaAlloc(a, len) arena_alloc(a, len)
#define afArenaResize(a, old_mem, old_size, new_size) arena_resize(a, old_mem, old_size, new_size)
#define afArenaFree(a, ptr) arena_free(a, ptr)
#define afArenaFree_all(a) arena_free_all(a)
#define afArenaDBGPrint(a, label) arena_debug_print(a, label);