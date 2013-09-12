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
#include "hamsterdb.h"
#include "endianswap.h"
#include "os.h"
#include "misc.h"
#include "metrics.h"

#include <boost/thread.hpp>
#include <ham/hamsterdb_int.h>

ham_env_t *Hamsterdb::ms_env;
ham_env_t *Hamsterdb::ms_remote_env;
ham_srv_t *Hamsterdb::ms_srv;
boost::mutex Hamsterdb::ms_mutex;

static int 
compare_keys(ham_db_t *db,
      const ham_u8_t *lhs, ham_size_t lhs_length, 
      const ham_u8_t *rhs, ham_size_t rhs_length)
{
  unsigned ulhs, urhs;

  (void)db;

  memcpy(&ulhs, lhs, sizeof(ulhs));
  memcpy(&urhs, rhs, sizeof(urhs));
  ulhs = ham_db2h32(ulhs);
  urhs = ham_db2h32(urhs);

  if (ulhs<urhs)
    return -1;
  if (ulhs == urhs)
    return 0;
  return 1;
}

Hamsterdb::~Hamsterdb()
{
  free(m_useralloc);
}

ham_status_t 
Hamsterdb::create_env()
{
  ham_u32_t flags = 0;
  ham_parameter_t params[6] = {{0, 0}};

  boost::mutex::scoped_lock lock(ms_mutex);
  if (ms_env)
    return (0);

  params[0].name = HAM_PARAM_CACHESIZE;
  params[0].value = m_config->cachesize;
  params[1].name = HAM_PARAM_PAGESIZE;
  params[1].value = m_config->pagesize;
  params[2].name = HAM_PARAM_MAX_DATABASES;
  params[2].value = 32;
  if (m_config->encryption) {
    params[3].name = HAM_PARAM_ENCRYPTION_KEY;
    params[3].value = (ham_u64_t)"1234567890123456";
  }

  flags |= m_config->inmemory ? HAM_IN_MEMORY : 0; 
  flags |= m_config->no_mmap ? HAM_DISABLE_MMAP : 0; 
  flags |= m_config->recovery ? HAM_ENABLE_RECOVERY : 0;
  flags |= m_config->cacheunlimited ? HAM_CACHE_UNLIMITED : 0;
  flags |= m_config->enable_transactions ? HAM_ENABLE_TRANSACTIONS : 0;
  flags |= m_config->use_writethrough ? HAM_ENABLE_FSYNC : 0;

  Os::unlink(DB_PATH "test-ham.db");

  timer t(this, timer::kMisc);

  ham_status_t st = ham_env_create(&ms_env, DB_PATH "test-ham.db", flags, 0664,
       &params[0]);
  if (st)
    return (st);

  // remote client/server? start the server, attach the environment and then
  // open the remote environment
  if (m_config->remote) {
    ms_remote_env = ms_env;
    ms_env = 0;

    ham_srv_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.port = 10123;
    ham_srv_init(&cfg, &ms_srv);
    ham_srv_add_env(ms_srv, ms_remote_env, "/env1.db");

    ham_u32_t flags = 0;
    flags |= m_config->duplicate ? HAM_ENABLE_DUPLICATE_KEYS : 0;
    st = ham_env_open(&ms_env, "ham://localhost:10123/env1.db", flags, 0);
  }

  return (st);
}

ham_status_t 
Hamsterdb::create_db()
{
  ham_status_t st;
  ham_parameter_t params[6] = {{0, 0}};

  timer t(this, timer::kMisc);

  params[0].name = HAM_PARAM_KEYSIZE;
  params[0].value = m_config->keysize;

  ham_u32_t flags = 0;

  flags |= m_config->duplicate ? HAM_ENABLE_DUPLICATE_KEYS : 0;
  if (!m_config->is_numeric())
    flags |= HAM_ENABLE_EXTENDED_KEYS;

  // remote + numeric? then create the database on server side FIRST,
  // because setting a custom compare function is now allowed on
  // remote databases
  if (m_config->is_numeric() && m_config->remote) {
    st = ham_env_create_db(ms_remote_env, &m_db, 1 + m_id, flags, &params[0]);
    if (st) {
      TRACE(("failed to create database %d: %d\n", 1 + m_id, st));
      return (st);
    }
    if (m_config->is_numeric()) {
      st = ham_db_set_compare_func(m_db, compare_keys);
      if (st)
        return (st);
    }
    // do not close, but open locally
    st = ham_env_open_db(ms_env, &m_db, 1 + m_id, 0, 0);
    if (st) {
      TRACE(("failed to create database %d: %d\n", 1 + m_id, st));
      return (st);
    }
  }
  else {
    st = ham_env_create_db(ms_env, &m_db, 1 + m_id, flags, &params[0]);
    if (st) {
      TRACE(("failed to create database %d: %d\n", 1 + m_id, st));
      return (st);
    }

    if (m_config->is_numeric()) {
      st = ham_db_set_compare_func(m_db, compare_keys);
      if (st)
        return (st);
    }
  }

  return (ham_cursor_create(&m_cursor, m_db, m_txn, 0));
}

ham_status_t 
Hamsterdb::open_env()
{
  ham_u32_t flags = 0;
  ham_parameter_t params[6] = {{0, 0}};

  timer t(this, timer::kMisc);

  params[0].name = HAM_PARAM_CACHESIZE;
  params[0].value = m_config->cachesize;
  if (m_config->encryption) {
    params[1].name = HAM_PARAM_ENCRYPTION_KEY;
    params[1].value = (ham_u64_t)"1234567890123456";
  }

  flags |= m_config->no_mmap ? HAM_DISABLE_MMAP : 0; 
  flags |= m_config->cacheunlimited ? HAM_CACHE_UNLIMITED : 0;
  flags |= m_config->enable_transactions ? HAM_ENABLE_TRANSACTIONS : 0;
  flags |= m_config->use_writethrough ? HAM_ENABLE_FSYNC : 0;

  ham_status_t st = ham_env_open(&ms_env, DB_PATH "test-ham.db",
                  flags, &params[0]);
  if (st)
    return (st);

  // remote client/server? start the server, attach the environment and then
  // open the remote environment
  if (m_config->remote) {
    ms_remote_env = ms_env;
    ms_env = 0;

    ham_srv_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.port = 10123;
    ham_srv_init(&cfg, &ms_srv);
    ham_srv_add_env(ms_srv, ms_remote_env, "/env1.db");

    ham_u32_t flags = 0;
    flags |= m_config->duplicate ? HAM_ENABLE_DUPLICATE_KEYS : 0;
    st = ham_env_open(&ms_env, "ham://localhost:10123/env1.db", flags, 0);
  }

  return (st);
}

ham_status_t 
Hamsterdb::open_db()
{
  ham_status_t st;

  timer t(this, timer::kMisc);

  // remote + numeric? then create the database on server side FIRST,
  // because setting a custom compare function is now allowed on
  // remote databases
  if (m_config->is_numeric() && m_config->remote) {
    st = ham_env_open_db(ms_remote_env, &m_db, 1 + m_id, 0, 0);
    if (st) {
      TRACE(("failed to open database %d: %d\n", 1 + m_id, st));
      return (st);
    }
    if (m_config->is_numeric()) {
      st = ham_db_set_compare_func(m_db, compare_keys);
      if (st)
        return (st);
    }
  }

  // now open locally
  st = ham_env_open_db(ms_env, &m_db, 1 + m_id, 0, 0);
  if (st) {
    TRACE(("failed to open database %d: %d\n", 1 + m_id, st));
    return (st);
  }
 
  if (!m_config->remote && m_config->is_numeric()) {
    st = ham_db_set_compare_func(m_db, compare_keys);
    if (st)
      return (st);
  }

  return (ham_cursor_create(&m_cursor, m_db, m_txn, 0));
}

ham_status_t 
Hamsterdb::close_txn()
{
  timer t(this, timer::kMisc);
  ham_status_t st = 0;

  if (m_cursor) {
    st = ham_cursor_close(m_cursor);
    if (st)
      return (st);
    m_cursor = 0;
  }

  if (m_txn) {
    st = ham_txn_commit(m_txn, 0);
    if (st)
      return (st);
    m_txn = 0;
  }

  return (st);
}

ham_status_t 
Hamsterdb::close_db()
{
  timer t(this, timer::kMisc);

  if (m_db)
    ham_db_close(m_db, HAM_AUTO_CLEANUP);
  m_db = 0;
  return 0;
}

ham_status_t 
Hamsterdb::close_env()
{
  timer t(this, timer::kMisc);

  if (ms_env)
    ham_env_close(ms_env, 0);
  ms_env = 0;
  if (ms_remote_env)
    ham_env_close(ms_remote_env, 0);
  ms_remote_env = 0;
  if (ms_srv)
    ham_srv_close(ms_srv);
  ms_srv = 0;
  return 0;
}

ham_status_t 
Hamsterdb::flush()
{
  timer t(this, timer::kMisc);

  return (ham_env_flush(ms_env, 0));
}

ham_status_t 
Hamsterdb::insert(ham_key_t *key, ham_record_t *record)
{
  ham_u32_t flags = 0;

  if (m_config->use_cursors) {
    flags |= m_config->hints & HAM_HINT_APPEND;

    if (m_config->overwrite)
      flags |= HAM_OVERWRITE;
    else if (m_config->duplicate) {
      flags |= HAM_DUPLICATE;
      flags |= m_config->dupe_flags;
    }

    timer t(this, timer::kInsert);
    return (ham_cursor_insert(m_cursor, key, record, flags));
  }
  else {
    if (m_config->overwrite)
      flags |= HAM_OVERWRITE;
    if (m_config->duplicate)
      flags |= HAM_DUPLICATE;

    timer t(this, timer::kInsert);
    return (ham_db_insert(m_db, m_txn, key, record, flags));
  }
}

ham_status_t 
Hamsterdb::erase(ham_key_t *key)
{
  ham_status_t st;
  ham_u32_t flags = 0;

  if (m_config->use_cursors) {
    timer t(this, timer::kErase);
    st = ham_cursor_find(m_cursor, key, 0, flags);
    if (st)
      return (st);
    return (ham_cursor_erase(m_cursor, flags));
  }
  else {
    timer t(this, timer::kErase);
    return (ham_db_erase(m_db, m_txn, key, flags));
  }
}

ham_status_t 
Hamsterdb::find(ham_key_t *key, ham_record_t *record)
{
  ham_u32_t flags = 0;

  if (m_config->direct_access && m_config->inmemory)
    flags |= HAM_DIRECT_ACCESS;

  if (!m_txn) {
    record->flags = HAM_RECORD_USER_ALLOC;
    record->data = m_useralloc;
  }

  if (m_config->use_cursors) {
    timer t(this, timer::kFind);
    return ham_cursor_find(m_cursor, key, record, flags);
  }
  else {
    timer t(this, timer::kFind);
    return ham_db_find(m_db, m_txn, key, record, flags);
  }
}

ham_status_t 
Hamsterdb::txn_begin()
{
  ham_status_t st;

  assert(m_txn == 0);

  if (m_cursor) {
    st = ham_cursor_close(m_cursor);
    if (st)
      return (st);
    m_cursor = 0;
  }

  {
  timer t(this, timer::kTxn);
  st = ham_txn_begin(&m_txn, ms_env, 0, 0, 0);
  if (st)
    return (st);
  }

  st = ham_cursor_create(&m_cursor, m_db, m_txn, 0);
  if (st) {
    TRACE(("failed to create cursor: %d\n", st));
    exit(-1);
  }

  return (0);
}

ham_status_t 
Hamsterdb::txn_commit()
{
  ham_status_t st;

  assert(m_txn != 0);

  if (m_cursor) {
    st = ham_cursor_close(m_cursor);
    if (st)
      return (st);
    m_cursor = 0;
  }

  timer t(this, timer::kTxn);
  st = ham_txn_commit(m_txn, 0);
  if (st)
    return (st);
  m_txn = 0;
  return (0);
}

ham_status_t 
Hamsterdb::check_integrity()
{
  return (ham_db_check_integrity(m_db, m_txn));
}

void *
Hamsterdb::create_cursor()
{
  ham_cursor_t *cursor;

  ham_status_t st = ham_cursor_create(&cursor, m_db, m_txn, 0);
  if (st) {
    TRACE(("failed to create cursor: %d\n", st));
    exit(-1);
  }

  return ((void *)cursor);
}

ham_status_t 
Hamsterdb::get_previous(void *cursor, ham_key_t *key, 
          ham_record_t *record, int flags)
{
  timer t(this, timer::kCursor);

  if (m_config->direct_access && m_config->inmemory)
    flags |= HAM_DIRECT_ACCESS;

  if (!m_txn) {
    record->flags = HAM_RECORD_USER_ALLOC;
    record->data = m_useralloc;
  }

  return (ham_cursor_move((ham_cursor_t *)cursor, key, record, 
            HAM_CURSOR_PREVIOUS | flags));
}

ham_status_t
Hamsterdb::get_next(void *cursor, ham_key_t *key, ham_record_t *record, 
        int flags)
{
  timer t(this, timer::kCursor);

  if (m_config->direct_access && m_config->inmemory)
    flags |= HAM_DIRECT_ACCESS;

  if (!m_txn) {
    record->flags = HAM_RECORD_USER_ALLOC;
    record->data = m_useralloc;
  }

  return (ham_cursor_move((ham_cursor_t *)cursor, key, record, 
            HAM_CURSOR_NEXT | flags));
}

void 
Hamsterdb::close_cursor(void *cursor)
{
  ham_status_t st = ham_cursor_close((ham_cursor_t *)cursor);
  if (st) {
    TRACE(("failed to close cursor: %d\n", st));
    exit(-1);
  }
}

void 
Hamsterdb::collect_metrics()
{
  Database::collect_metrics();

  ham_env_metrics_t metrics;
  ham_env_get_metrics(ms_env, &metrics);

  Metrics::get_instance()->add_metric(get_name(), "mem-total-allocations", 
            metrics.mem_total_allocations);
  Metrics::get_instance()->add_metric(get_name(), "mem-peak-usage", 
            metrics.mem_peak_usage);
  Metrics::get_instance()->add_metric(get_name(), "page-count-fetched", 
            metrics.page_count_fetched);
  Metrics::get_instance()->add_metric(get_name(), "page-count-flushed", 
            metrics.page_count_flushed);
  Metrics::get_instance()->add_metric(get_name(), "page-count-type-index", 
            metrics.page_count_type_index);
  Metrics::get_instance()->add_metric(get_name(), "page-count-type-blob", 
            metrics.page_count_type_blob);
  Metrics::get_instance()->add_metric(get_name(), "page-count-type-freelist", 
            metrics.page_count_type_freelist);
  Metrics::get_instance()->add_metric(get_name(), "freelist-hits",
            metrics.freelist_hits);
  Metrics::get_instance()->add_metric(get_name(), "freelist-misses",
            metrics.freelist_misses);
  Metrics::get_instance()->add_metric(get_name(), "cache-hits",
            metrics.cache_hits);
  Metrics::get_instance()->add_metric(get_name(), "cache-misses",
            metrics.cache_misses);
  Metrics::get_instance()->add_metric(get_name(), "blob-total-allocated",
            metrics.blob_total_allocated);
  Metrics::get_instance()->add_metric(get_name(), "blob-total-read",
            metrics.blob_total_read);
  Metrics::get_instance()->add_metric(get_name(), "blob-direct-read",
            metrics.blob_direct_read);
  Metrics::get_instance()->add_metric(get_name(), "blob-direct-written",
            metrics.blob_direct_written);
  Metrics::get_instance()->add_metric(get_name(), "blob-direct-allocated",
            metrics.blob_direct_allocated);
  Metrics::get_instance()->add_metric(get_name(), "extkey-cache-hits",
            metrics.extkey_cache_hits);
  Metrics::get_instance()->add_metric(get_name(), "extkey-cache-misses",
            metrics.extkey_cache_misses);
}

