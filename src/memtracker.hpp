/**
 * Copyright (C) 2005-2012 Christoph Rupp (chris@crupp.de).
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
#include "../../hamsterdb/src/mem.h"
#include "metrics.hpp"
#include <boost/thread.hpp>

#define MAGIC           0x12345678
#define OFFSETOF(type, member) ((size_t) &((type *)0)->member)

typedef boost::mutex::scoped_lock ScopedLock;

typedef struct memdesc_t
{
  unsigned int size;
  unsigned int magic;
  char data[1];
} memdesc_t;

class TrackingAllocator : public ham::Allocator
{
  public:
    TrackingAllocator()
      : Allocator() {
    }

    virtual void *alloc(ham_size_t size) {
      memdesc_t *desc = (memdesc_t *)::malloc(sizeof(*desc)+size-1);
      if (!desc)
        return (0);
      memset(desc, 0, sizeof(*desc));
      desc->size = size;
      desc->magic = MAGIC;

      ScopedLock(ms_mutex);
      ms_mem_total += size;
      ms_mem_allocs++;
      ms_mem_current += size;
      if (ms_mem_current > ms_mem_peak)
        ms_mem_peak = ms_mem_current;

      return (desc->data);
    }

    virtual void free(const void *ptr) {
      memdesc_t *desc = get_descriptor(ptr);
      verify_mem_desc(desc);

      ham_size_t s = desc->size;
      ::free(desc);

      ScopedLock(ms_mutex);
      ms_mem_current -= s;
    }

    virtual void *realloc(const void *ptr, ham_size_t size) {
      /* ptr is NULL -> always alloc */
      if (!ptr)
        return (alloc(size));

      /* only resize if requested new size is > old size */
      memdesc_t *desc = get_descriptor(ptr);
      if (size < desc->size)
        return ((void *)ptr);

      /* otherwise just realloc the existing pointer */
      desc = (memdesc_t *)::realloc(desc, sizeof(*desc) + size - 1);
      desc->size = size;
      return (desc->data);
    }

    static unsigned long get_num_allocs() { 
      ScopedLock(ms_mutex);
      return ms_mem_allocs; 
    }

    static unsigned long get_peak_bytes() { 
      ScopedLock(ms_mutex);
      return ms_mem_peak; 
    }

    static unsigned long get_total_bytes() { 
      ScopedLock(ms_mutex);
      return ms_mem_total; 
    }

  private:
    memdesc_t *get_descriptor(const void *p) {
      return ((memdesc_t *)((char *)p - OFFSETOF(memdesc_t, data)));
    }

    void verify_mem_desc(memdesc_t *desc) {
      if (desc->size == 0)
        throw std::out_of_range("memory blob size is 0");
      if (desc->magic != MAGIC)
        throw std::out_of_range("memory blob descriptor is corrupt");
    }

    static unsigned long ms_mem_allocs;
    static unsigned long ms_mem_peak;
    static unsigned long ms_mem_current;
    static unsigned long ms_mem_total;
    static boost::mutex ms_mutex;
};

