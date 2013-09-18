/*
 * Copyright (C) 2005-2013 Christoph Rupp (chris@crupp.de).
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * See files COPYING.* for License information.
 */

#ifndef METRICS_H__
#define METRICS_H__

#include <ham/hamsterdb_int.h>

struct Metrics {
  uint64_t insert_ops; 
  uint64_t erase_ops; 
  uint64_t find_ops; 
  uint64_t other_ops; 
  uint64_t insert_bytes; 
  uint64_t find_bytes; 
  double elapsed_wallclock_seconds;
  double insert_latency_min;
  double insert_latency_max;
  double insert_latency_avg;
  double erase_latency_min;
  double erase_latency_max;
  double erase_latency_avg;
  double find_latency_min;
  double find_latency_max;
  double find_latency_avg;
  double txn_commit_latency_min;
  double txn_commit_latency_max;
  double txn_commit_latency_avg;
  ham_env_metrics_t hamster_metrics;
};

#endif /* METRICS_H__ */
