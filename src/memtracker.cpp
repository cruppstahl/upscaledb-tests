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

#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include "memtracker.hpp"

#define MAGIC     0x12345678

#define OFFSETOF(type, member) ((size_t) &((type *)0)->member)

static memdesc_t *
get_descriptor(const void *p)
{
    return ((memdesc_t *)((char *)p-OFFSETOF(memdesc_t, data)));
}

static void 
verify_mem_desc(memdesc_t *desc)
{
    if (desc->size==0)
        throw std::out_of_range("memory blob size is 0");
    if (desc->magic!=MAGIC)
        throw std::out_of_range("memory blob descriptor is corrupt");
}

void *
alloc_impl(mem_allocator_t *self, const char *file, int line, ham_u32_t size)
{
    memtracker_t *mt=(memtracker_t *)self;
    memtracker_priv_t *priv=(memtracker_priv_t *)mt->priv;
    memdesc_t *desc=(memdesc_t *)malloc(sizeof(*desc)+size-1);
    if (!desc)
        return (0);
    memset(desc, 0, sizeof(*desc));
    desc->size=size;
    desc->magic=MAGIC;

    priv->allocs++;
    priv->current+=size;
    if (priv->current>priv->peak)
        priv->peak=priv->current;

    return (desc->data);
}

void 
free_impl(mem_allocator_t *self, const char *file, int line, const void *ptr)
{
    memtracker_t *mt=(memtracker_t *)self;
    memtracker_priv_t *priv=(memtracker_priv_t *)mt->priv;
    memdesc_t *desc=get_descriptor(ptr);
    verify_mem_desc(desc);

    priv->current-=desc->size;
    free(desc);
}

void *
realloc_impl(mem_allocator_t *self, const char *file, int line, 
        const void *ptr, ham_size_t size)
{
    /* ptr is NULL -> always alloc */
    if (!ptr)
        return (alloc_impl(self, file, line, size));

    /* only resize if requested new size is > old size */
    memdesc_t *desc=get_descriptor(ptr);
    if (size<desc->size)
        return ((void *)ptr);

    /* otherwise just realloc the existing pointer */
    return realloc(desc, sizeof(*desc)+size-1);
}

void 
close_impl(mem_allocator_t *self)
{
    (void)self;
}

memtracker_t *
memtracker_new(void)
{
    static memtracker_t m;
    static memtracker_priv_t p;
    memset(&m, 0, sizeof(m));
    memset(&p, 0, sizeof(p));
    m.alloc=alloc_impl;
    m.free =free_impl;
    m.realloc=realloc_impl;
    m.close=close_impl;
    m.priv=&p;
    return (&m);
}

unsigned long
memtracker_get_peak(memtracker_t *mt)
{
    memtracker_priv_t *priv=(memtracker_priv_t *)mt->priv;
    return (priv->peak);
}

unsigned long
memtracker_get_allocs(memtracker_t *mt)
{
    memtracker_priv_t *priv=(memtracker_priv_t *)mt->priv;
    return (priv->allocs);
}

unsigned long
memtracker_get_current(memtracker_t *mt)
{
    memtracker_priv_t *priv=(memtracker_priv_t *)mt->priv;
    return (priv->current);
}
