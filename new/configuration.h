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


#ifndef CONFIGURATION_H__
#define CONFIGURATION_H__

#include <malloc.h>
#include <boost/thread.hpp>

struct Configuration
{
  enum {
    kKeyBinary = 0,
    kKeyUint8,
    kKeyUint16,
    kKeyUint32,
    kKeyUint64
  };

  enum {
    kDistributionRandom = 0,
    kDistributionAscending,
    kDistributionDescending,
    kDistributionZipfian
  };

  Configuration()
  : profile(true), verbose(0), progress(false), filename(0), quiet(false),
    key_type(kKeyBinary), distribution(kDistributionRandom), seed(0),
    limit_ops(1000000ull), key_size(16), key_is_fixed_size(false),
    open(false), erase_pct(0), find_pct(0), limit_seconds(5) {
  }

  bool profile;
  unsigned verbose;
  bool progress;
  const char *filename;
  bool quiet;
  int key_type;
  int distribution;
  int seed;
  uint64_t limit_ops;
  int key_size;
  bool key_is_fixed_size;
  bool open;
  int erase_pct;
  int find_pct;
  uint32_t limit_seconds;
};

#endif /* CONFIGURATION_H__ */
