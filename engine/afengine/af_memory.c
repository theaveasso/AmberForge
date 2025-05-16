#include "af_memory.h"

#include <memory.h>

Internal uintptr_t align_forward(uintptr_t ptr, size_t align) {
    uintptr_t p, a, mod;
    assert(is_power_of_two(align));

    p   = ptr;
    a   = (uintptr_t) align;
    mod = p & (a - 1);// same as (p%a) but faster as 'a' is a power of two

    if (mod != 0) { p += a - mod; }// if 'p' addr is not aligned, push the addr to the next value which is aligned
    return p;
}

void arena_init(AFarena *a, void *backing_buffer, size_t backing_buffer_len) {
    a->buf         = (unsigned char *) backing_buffer;
    a->buf_len     = backing_buffer_len;
    a->prev_offset = 0;
    a->curr_offset = 0;
}

void *arena_alloc(AFarena *a, size_t len) {
    return arena_alloc_align(a, len, AF_DEFAULT_ALIGNMENT);
}

void *arena_alloc_align(AFarena *a, size_t len, size_t align) {
    // align 'curr_offset' forward to the specified alignment
    uintptr_t curr_ptr = (uintptr_t) a->buf + (uintptr_t) a->curr_offset;
    uintptr_t offset   = align_forward(curr_ptr, align);
    offset -= (uintptr_t) a->buf;

    // check to see if the backing memory has space left
    if (offset + len <= a->buf_len)
    {
        void *ptr      = &a->buf[offset];
        a->prev_offset = offset;
        a->curr_offset = offset + len;

        // zero new memory by default
        memset(ptr, 0, len);
        return ptr;
    }

    // return 'NULL' if the arena is out of memory (or handle differently)
    return NULL;
}

void *arena_resize(AFarena *a, void *oldmem, size_t osize, size_t nsize) {
    return arena_resize_align(a, oldmem, osize, nsize, AF_DEFAULT_ALIGNMENT);
}

void *arena_resize_align(AFarena *a, void *oldmem, size_t osize, size_t nsize, size_t align) {
    unsigned char *old_mem = (unsigned char *) oldmem;
    assert(is_power_of_two(align));

    if (old_mem == NULL || osize == 0)
    {
        return arena_alloc(a, nsize);
    }
    else if (a->buf <= old_mem && old_mem < a->buf + a->buf_len)
    {
        if (a->buf + a->prev_offset == old_mem)
        {
            a->curr_offset = a->prev_offset + nsize;
            if (nsize > osize) { memset(&a->buf[a->curr_offset], 0, nsize - osize); }
            return old_mem;
        }
    }

    void *new_mem    = arena_alloc(a, nsize);
    size_t copy_size = osize < nsize ? osize : nsize;
    memmove(new_mem, oldmem, copy_size);
    return new_mem;
}

void arena_free(AFarena *a, void *ptr) {
    (void) a;
    (void) ptr;
}

void arena_free_all(AFarena *a) {
    a->prev_offset = 0;
    a->curr_offset = 0;
}