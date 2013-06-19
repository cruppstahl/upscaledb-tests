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
  typedef std::map<std::string, std::map<std::string, unsigned long> > MetricMap;
  public:
    void add_metric(const char *idx, const char *name, unsigned long value) {
      ScopedLock lock(m_mutex);
      m_metrics[idx][name] = value;
    }

    void print() {
      ScopedLock lock(m_mutex);
      for (MetricMap::iterator it = m_metrics.begin(); 
            it != m_metrics.end(); ++it) {
        std::map<std::string, unsigned long> &m = it->second;
        std::map<std::string, unsigned long>::iterator it2;
        if (m.empty())
          continue;
        printf("%s\n", it->first.c_str());
        for (it2 = m.begin(); it2 != m.end(); ++it2) {
          if (!strstr(it2->first.c_str(), "perf-"))
            printf("\t%s\t\t%lu\n", it2->first.c_str(), it2->second);
          else
            printf("\t%s\t\t%f\n", it2->first.c_str(), 
                      (double)it2->second / 1000000000.);
        }
      }
    }

    static Metrics *get_instance() {
      if (!instance)
        instance = new Metrics();
      return instance;
    }

    static Metrics *instance;

    ham_env_metrics_t *get_ham_metrics() {
      return (&m_ham_metrics);
    }

  private:
    Metrics() {
    }

    boost::mutex m_mutex;

    ham_env_metrics_t m_ham_metrics;

    MetricMap m_metrics;
};

#endif // METRICS_HPP__

