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

#include <ham/hamsterdb.h>
#include <ham/mem.h>
#include "../../hamsterdb/src/mem.h"
#include "metrics.hpp"

#define MAGIC           0x12345678
#define OFFSETOF(type, member) ((size_t) &((type *)0)->member)

typedef struct memdesc_t
{
    unsigned int size;
    unsigned int magic;
    char data[1];
} memdesc_t;

class TrackingAllocator : public Allocator
{
  public:
    TrackingAllocator()
    : Allocator() {
    }

    virtual void *alloc(ham_size_t size) {
        memdesc_t *desc=(memdesc_t *)::malloc(sizeof(*desc)+size-1);
        if (!desc)
            return (0);
        memset(desc, 0, sizeof(*desc));
        desc->size=size;
        desc->magic=MAGIC;

        Metrics::get_instance()->inc_mem_total(size);
        Metrics::get_instance()->inc_mem_allocs();
        Metrics::get_instance()->inc_mem_current(size);

        return (desc->data);
    }

    virtual void free(const void *ptr) {
        memdesc_t *desc=get_descriptor(ptr);
        verify_mem_desc(desc);

        Metrics::get_instance()->dec_mem_current(desc->size);
        ::free(desc);
    }

    virtual void *realloc(const void *ptr, ham_size_t size) {
        /* ptr is NULL -> always alloc */
        if (!ptr)
            return (alloc(size));

        /* only resize if requested new size is > old size */
        memdesc_t *desc=get_descriptor(ptr);
        if (size<desc->size)
            return ((void *)ptr);

        /* otherwise just realloc the existing pointer */
        desc=(memdesc_t  *)::realloc(desc, sizeof(*desc)+size-1);
        desc->size=size;
        return (desc->data);
    }

  private:
    memdesc_t *get_descriptor(const void *p) {
        return ((memdesc_t *)((char *)p-OFFSETOF(memdesc_t, data)));
    }

    void verify_mem_desc(memdesc_t *desc) {
        if (desc->size==0)
            throw std::out_of_range("memory blob size is 0");
        if (desc->magic!=MAGIC)
            throw std::out_of_range("memory blob descriptor is corrupt");
    }
};

