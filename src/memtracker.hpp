/**
 * Copyright (C) 2005-2008 Christoph Rupp (chris@crupp.de).
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * See files COPYING.* for License information.
 */

#include <ham/hamsterdb.h>

typedef struct memdesc_t
{
    unsigned int size;
    unsigned int magic;
    char data[1];
} memdesc_t;

typedef struct 
{
    unsigned long peak;
    unsigned long current;
    unsigned long allocs;
} memtracker_priv_t;

/* copied from mem.h */
typedef void *(*alloc_func_t)(struct mem_allocator_t *self, const char *file, 
                   int line, ham_size_t size);
typedef void  (*free_func_t) (struct mem_allocator_t *self, const char *file, 
                   int line, const void *ptr);
typedef void *(*realloc_func_t) (struct mem_allocator_t *self, const char *file, 
                   int line, const void *ptr, ham_size_t size);
typedef void  (*close_func_t)(struct mem_allocator_t *self);
struct mem_allocator_t
{
    alloc_func_t alloc;
    free_func_t  free;
    realloc_func_t realloc;
    close_func_t close;
    void *priv;
};


typedef struct 
{
    alloc_func_t alloc;
    free_func_t  free;
    realloc_func_t realloc;
    close_func_t close;

    memtracker_priv_t *priv;

} memtracker_t;

extern memtracker_t *
memtracker_new(void);

extern unsigned long
memtracker_get_peak(memtracker_t *mt);

extern unsigned long
memtracker_get_current(memtracker_t *mt);

extern unsigned long
memtracker_get_allocs(memtracker_t *mt);


