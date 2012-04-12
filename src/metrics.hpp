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

#ifndef METRICS_HPP__
#define METRICS_HPP__

#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <string>
#include <vector>
#include <map>
#include <boost/thread.hpp>

typedef boost::mutex::scoped_lock ScopedLock;

class Metrics
{
  public:
    void add_metric(int idx, const char *name, unsigned long value) {
        ScopedLock lock(m_mutex);
        m_metrics[idx][name]=value;
    }

    void print() {
        ScopedLock lock(m_mutex);
        for (int i=0; i<2; i++) {
            std::map<std::string, unsigned long> &m=m_metrics[i];
            std::map<std::string, unsigned long>::iterator it;
            if (!m.empty())
                printf("%s\n", i ? "berkeleydb" : "hamsterdb");
            for (it=m.begin(); it!=m.end(); ++it)
                printf("\t%s\t\t%lu\n", it->first.c_str(), it->second);
        }
    }

    static Metrics *get_instance() {
        if (!instance)
            instance=new Metrics();
        return instance;
    }

    static Metrics *instance;

  private:
    Metrics()
      : m_metrics(100) {
    }

    unsigned long m_mem_allocs;
    unsigned long m_mem_peak;
    unsigned long m_mem_current;
    unsigned long m_mem_total;
    boost::mutex m_mutex;

    std::vector<std::map<std::string, unsigned long> > m_metrics;
};


#endif // METRICS_HPP__

