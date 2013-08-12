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


#ifndef CONFIG_HPP__
#define CONFIG_HPP__

#include <malloc.h>
#include <boost/thread.hpp>

#define USER_MALLOC_KEYRECSIZE  (1024*1024*64)


#define DB_PATH ""


typedef struct config
{
  config()
  {
    profile = true;
    verbose = 0;
    overwrite = false;
    duplicate = false;
    inmemory = false;
    use_cursors = false;
    numeric = false;
    progress = false;
    no_mmap = false;
    keysize = 0;
    pagesize = 0;
    cachesize = 0;
    reopen = false;
    filename = 0;
    fullcheck_find = false;
    fullcheck_backwards = false;
    dupe_flags = 0;
    recovery = false;
    cacheunlimited = false;
    hints = 0;
    direct_access = false;
    enable_transactions = false;
    txn_group = 0;
    use_writethrough = false;
    quiet = false;
    no_bdb = false;
    num_threads = 1;
    encryption = false;
    remote = false;
  }

  void set_numeric(bool b) {
    boost::mutex::scoped_lock lock(m_mutex);
    numeric = b;
  }
  bool is_numeric() {
    boost::mutex::scoped_lock lock(m_mutex);
    return numeric;
  }

  bool profile;
  unsigned verbose;
  bool overwrite;
  bool duplicate;
  bool inmemory;
  bool use_cursors;
  bool progress;
  bool no_mmap;
  unsigned keysize;
  unsigned pagesize;
  unsigned cachesize;
  bool reopen;
  const char *filename;
  bool fullcheck_find;
  bool fullcheck_backwards;
  unsigned dupe_flags;
  unsigned hints;
  bool recovery;
  bool cacheunlimited;
  bool direct_access;
  bool enable_transactions;
  unsigned txn_group;
  bool use_writethrough;
  bool quiet;
  bool no_bdb;
  unsigned num_threads;
  bool encryption;
  bool remote;
  boost::mutex m_mutex;

  private:
  bool numeric;
} config;

#endif /* CONFIG_HPP__ */
