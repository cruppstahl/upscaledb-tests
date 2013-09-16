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

#include <string>
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
    kRecordVariable = 0,
    kRecordFixed
  };

  enum {
    kFullcheckDefault = 0,
    kFullcheckFind,
    kFullcheckReverse,
    kFullcheckNone
  };

  enum {
    kDistributionRandom = 0,
    kDistributionAscending,
    kDistributionDescending,
    kDistributionZipfian
  };

  enum {
    kDuplicateDisabled = 0,
    kDuplicateFirst,
    kDuplicateLast
  };

  Configuration()
    : profile(true), verbose(0), no_progress(false), reopen(true),
      quiet(false), key_type(kKeyBinary), record_type(kRecordVariable),
      distribution(kDistributionRandom), seed(0), limit_ops(0),
      limit_seconds(0), limit_bytes(1024 * 1024 * 100), key_size(16),
      key_is_fixed_size(false), rec_size(1024), open(false), erase_pct(10),
      find_pct(40), use_encryption(false), use_remote(false),
      duplicate(kDuplicateDisabled), overwrite(false), transactions_nth(0),
      use_fsync(false), inmemory(false), use_recovery(false),
      use_transactions(false), no_mmap(false), cacheunlimited(false),
      cachesize(0), hints(0), pagesize(0), num_threads(1), use_cursors(false),
      direct_access(false), no_berkeleydb(false), no_hamsterdb(false),
      fullcheck(kFullcheckDefault) {
  }

  bool profile;
  unsigned verbose;
  bool no_progress;
  bool reopen;
  std::string filename;
  bool quiet;
  int key_type;
  int record_type;
  int distribution;
  long seed;
  uint64_t limit_ops;
  uint64_t limit_seconds;
  uint64_t limit_bytes;
  int key_size;
  bool key_is_fixed_size;
  int rec_size;
  bool open;
  int erase_pct;
  int find_pct;
  bool use_encryption;
  bool use_remote;
  int duplicate;
  bool overwrite;
  int transactions_nth;
  bool use_fsync;
  bool inmemory;
  bool use_recovery;
  bool use_transactions;
  bool no_mmap;
  bool cacheunlimited;
  int cachesize;
  int hints;
  int pagesize;
  int num_threads;
  bool use_cursors;
  bool direct_access;
  bool no_berkeleydb;
  bool no_hamsterdb;
  int fullcheck;
  std::string tee_file;
};

#endif /* CONFIGURATION_H__ */
