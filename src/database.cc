/**
 * Copyright (C) 2005-2013 Christoph Rupp (chris@crupp.de).
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * See files COPYING.* for License information.
 */


#include "porting.h"
#include "configuration.h"
#include "database.h"
#include "metrics.h"

void 
Database::collect_metrics()
{
  static const char *s[] = {
    "perf-misc  \t", 
    "perf-insert\t", 
    "perf-erase \t", 
    "perf-find  \t", 
    "perf-txn   \t", 
    "perf-cursor\t"
  };

  float f, total = 0;

  for (int i = 0; i < 5; i++) {
    f = (float)m_profile[i];
    total += f;
    f *= 1000000.f;
    Metrics::get_instance()->add_metric(get_name(), s[i], (unsigned long)f);
  }

  total *= 1000000.f;
  Metrics::get_instance()->add_metric(get_name(), "perf-total", 
      (unsigned long)total);
}

