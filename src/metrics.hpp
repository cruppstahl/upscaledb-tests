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

#ifndef METRICS_HPP__
#define METRICS_HPP__

class Metrics
{
  public:
    void inc_mem_allocs() {
        m_mem_allocs++;
    }

    void inc_mem_current(unsigned size) {
        m_mem_current+=size;
        if (m_mem_current>m_mem_peak)
            m_mem_peak=m_mem_current;
    }

    void dec_mem_current(unsigned size) {
        m_mem_current-=size;
    }

    void inc_mem_total(unsigned size) {
        m_mem_total+=size;
    }

    void print_metric(const char *name, unsigned long value) {
        printf("\t%s\t\t%lu\n", name, value);
    }

    void print() {
        print_metric("mem-num-allocs", m_mem_allocs);
        print_metric("mem-peak-bytes", m_mem_peak);
        print_metric("mem-total-bytes", m_mem_total);
    }

    static Metrics *get_instance() {
        if (!instance)
            instance=new Metrics();
        return instance;
    }

    static Metrics *instance;

  private:
    Metrics()
      : m_mem_allocs(0), m_mem_peak(0), m_mem_current(0), m_mem_total(0) {
    }

    unsigned long m_mem_allocs;
    unsigned long m_mem_peak;
    unsigned long m_mem_current;
    unsigned long m_mem_total;
};


#endif // METRICS_HPP__

