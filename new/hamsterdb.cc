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

#include <iostream>
#include <boost/filesystem.hpp>

#include <ham/hamsterdb_int.h>

#include "configuration.h"
#include "misc.h"
#include "hamsterdb.h"

ham_env_t *HamsterDatabase::ms_env = 0;
ham_env_t *HamsterDatabase::ms_remote_env = 0;
ham_srv_t *HamsterDatabase::ms_srv = 0;

ham_status_t
HamsterDatabase::create(Configuration *config)
{
  ham_u32_t flags = 0;
  ham_parameter_t params[6] = {{0, 0}};

  params[0].name = HAM_PARAM_CACHESIZE;
  params[0].value = config->cachesize;
  params[1].name = HAM_PARAM_PAGESIZE;
  params[1].value = 0; // TODO config->pagesize;
  params[2].name = HAM_PARAM_MAX_DATABASES;
  params[2].value = 32; // for up to 32 threads
  if (config->use_encryption) {
    params[3].name = HAM_PARAM_ENCRYPTION_KEY;
    params[3].value = (ham_u64_t)"1234567890123456";
  }

  flags |= config->inmemory ? HAM_IN_MEMORY : 0; 
  flags |= config->no_mmap ? HAM_DISABLE_MMAP : 0; 
  flags |= config->use_recovery ? HAM_ENABLE_RECOVERY : 0;
  flags |= config->cacheunlimited ? HAM_CACHE_UNLIMITED : 0;
  flags |= config->use_transactions ? HAM_ENABLE_TRANSACTIONS : 0;
  flags |= config->use_fsync ? HAM_ENABLE_FSYNC : 0;

  boost::filesystem::remove("test-ham.db");

  ham_status_t st = ham_env_create(&ms_env, "test-ham.db", flags, 0664,
                  &params[0]);
  if (st) {
    print_error("ham_env_create", st);
    return (st);
  }

  // remote client/server? start the server, attach the environment and then
  // open the remote environment
  if (config->use_remote) {
    ms_remote_env = ms_env;
    ms_env = 0;

    ham_srv_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.port = 10123;
    ham_srv_init(&cfg, &ms_srv);
    ham_srv_add_env(ms_srv, ms_remote_env, "/env1.db");

    ham_u32_t flags = 0;
    flags |= config->duplicate ? HAM_ENABLE_DUPLICATES : 0;
    st = ham_env_open(&ms_env, "ham://localhost:10123/env1.db", flags, 0);
    if (st)
      print_error("ham_env_open", st);
  }

  return (st);
}

ham_status_t
HamsterDatabase::open(Configuration *config)
{
  ham_u32_t flags = 0;
  ham_parameter_t params[6] = {{0, 0}};

  params[0].name = HAM_PARAM_CACHESIZE;
  params[0].value = config->cachesize;
  if (config->use_encryption) {
    params[1].name = HAM_PARAM_ENCRYPTION_KEY;
    params[1].value = (ham_u64_t)"1234567890123456";
  }

  flags |= config->no_mmap ? HAM_DISABLE_MMAP : 0; 
  flags |= config->cacheunlimited ? HAM_CACHE_UNLIMITED : 0;
  flags |= config->use_transactions ? HAM_ENABLE_TRANSACTIONS : 0;
  flags |= config->use_fsync ? HAM_ENABLE_FSYNC : 0;

  ham_status_t st = ham_env_open(&ms_env, "test-ham.db", flags, &params[0]);
  if (st) {
    print_error("ham_env_open", st);
    return (st);
  }

  // remote client/server? start the server, attach the environment and then
  // open the remote environment
  if (config->use_remote) {
    ms_remote_env = ms_env;
    ms_env = 0;

    ham_srv_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.port = 10123;
    ham_srv_init(&cfg, &ms_srv);
    ham_srv_add_env(ms_srv, ms_remote_env, "/env1.db");

    ham_u32_t flags = 0;
    flags |= config->duplicate ? HAM_ENABLE_DUPLICATES : 0;
    st = ham_env_open(&ms_env, "ham://localhost:10123/env1.db", flags, 0);
    if (st)
      print_error("ham_env_open", st);
  }

  return (st);
}

ham_status_t
HamsterDatabase::close()
{
  if (ms_env)
    ham_env_close(ms_env, 0);
  ms_env = 0;
  if (ms_remote_env)
    ham_env_close(ms_remote_env, 0);
  ms_remote_env = 0;
  if (ms_srv)
    ham_srv_close(ms_srv);
  ms_srv = 0;
  return (0);
}

ham_status_t
HamsterDatabase::do_create_db()
{
  ham_status_t st;
  ham_parameter_t params[6] = {{0, 0}};

  params[0].name = 0; // TODO HAM_PARAM_KEYSIZE;
  params[0].value = 0; // TODO m_config->keysize;

  ham_u32_t flags = 0;

  flags |= m_config->duplicate ? HAM_ENABLE_DUPLICATES : 0;
  //if (m_config->key != Configuration::kKeyNumeric)
    //flags |= HAM_ENABLE_EXTENDED_KEYS;

  st = ham_env_create_db(ms_env, &m_db, 1 + m_id, flags, &params[0]);
  if (st) {
    ERROR(("failed to create database %d: %d\n", 1 + m_id, st));
    return (st);
  }

  return (0);
}

ham_status_t
HamsterDatabase::do_open_db()
{
  ham_status_t st;

  st = ham_env_open_db(ms_env, &m_db, 1 + m_id, 0, 0);
  if (st)
    print_error("ham_env_open_db", st);
 
  return (st);
}

ham_status_t
HamsterDatabase::do_close_db()
{
  if (m_db)
    ham_db_close(m_db, HAM_AUTO_CLEANUP);
  m_db = 0;
  return (0);
}

ham_status_t
HamsterDatabase::do_flush()
{
  return (ham_env_flush(ms_env, 0));
}

ham_status_t
HamsterDatabase::do_insert(Transaction *txn, ham_key_t *key,
                ham_record_t *record)
{
  ham_u32_t flags = m_config->hints;

  if (m_config->overwrite)
    flags |= HAM_OVERWRITE;
  else if (m_config->duplicate)
    flags |= HAM_DUPLICATE;

  return (ham_db_insert(m_db, (ham_txn_t *)txn, key, record, flags));
}

ham_status_t
HamsterDatabase::do_erase(Transaction *txn, ham_key_t *key)
{
  return (ham_db_erase(m_db, (ham_txn_t *)txn, key, 0));
}

ham_status_t
HamsterDatabase::do_find(Transaction *txn, ham_key_t *key, ham_record_t *record)
{
  ham_u32_t flags = 0;

  // if (m_config->direct_access && m_config->inmemory)
    // flags |= HAM_DIRECT_ACCESS;

#if 0
  if (!m_txn) {
    record->flags = HAM_RECORD_USER_ALLOC;
    record->data = m_useralloc;
  }
#endif

  return (ham_db_find(m_db, (ham_txn_t *)txn, key, record, flags));
}

ham_status_t
HamsterDatabase::do_check_integrity(Transaction *txn)
{
  return (ham_db_check_integrity(m_db, (ham_txn_t *)txn));
}

Database::Transaction *
HamsterDatabase::do_txn_begin()
{
  ham_txn_t *txn;
  ham_status_t st = ham_txn_begin(&txn, ms_env, 0, 0, 0);
  if (st) {
    print_error("ham_txn_begin", st);
    return (0);
  }
  return ((Database::Transaction *)txn);
}

ham_status_t
HamsterDatabase::do_txn_commit(Transaction *txn)
{
  ham_status_t st = ham_txn_commit((ham_txn_t *)txn, 0);
  if (st)
    print_error("ham_txn_commit", st);
  return (st);
}

ham_status_t
HamsterDatabase::do_txn_abort(Transaction *txn)
{
  ham_status_t st = ham_txn_abort((ham_txn_t *)txn, 0);
  if (st)
    print_error("ham_txn_abort", st);
  return (st);
}

Database::Cursor *
HamsterDatabase::do_cursor_create(Transaction *txn)
{
  ham_cursor_t *cursor;

  ham_status_t st = ham_cursor_create(&cursor, m_db, (ham_txn_t *)txn, 0);
  if (st) {
    TRACE(("failed to create cursor: %d\n", st));
    exit(-1);
  }

  return ((Database::Cursor *)cursor);
}

ham_status_t
HamsterDatabase::do_cursor_insert(Cursor *cursor, ham_key_t *key,
                ham_record_t *record)
{
  ham_u32_t flags = m_config->hints;

  if (m_config->overwrite)
    flags |= HAM_OVERWRITE;
  if (m_config->duplicate == Configuration::kDuplicateFirst)
    flags |= HAM_DUPLICATE | HAM_DUPLICATE_INSERT_FIRST;
  else if (m_config->duplicate == Configuration::kDuplicateLast)
    flags |= HAM_DUPLICATE | HAM_DUPLICATE_INSERT_LAST;

  return (ham_cursor_insert((ham_cursor_t *)cursor, key, record, flags));
}

ham_status_t
HamsterDatabase::do_cursor_erase(Cursor *cursor, ham_key_t *key)
{
  ham_status_t st = ham_cursor_find((ham_cursor_t *)cursor, key, 0, 0);
  if (st)
    return (st);
  return (ham_cursor_erase((ham_cursor_t *)cursor, 0));
}

ham_status_t
HamsterDatabase::do_cursor_find(Cursor *cursor, ham_key_t *key,
                    ham_record_t *record)
{
  return (ham_cursor_find((ham_cursor_t *)cursor, key, record, 0));
}

ham_status_t
HamsterDatabase::do_cursor_get_previous(Cursor *cursor, ham_key_t *key, 
                    ham_record_t *record)
{
  ham_u32_t flags = 0;

  // if (m_config->direct_access && m_config->inmemory)
    // flags |= HAM_DIRECT_ACCESS;

#if 0
  if (!m_txn) {
    record->flags = HAM_RECORD_USER_ALLOC;
    record->data = m_useralloc;
  }
#endif

  return (ham_cursor_move((ham_cursor_t *)cursor, key, record,
                          HAM_CURSOR_PREVIOUS | flags));
}

ham_status_t
HamsterDatabase::do_cursor_get_next(Cursor *cursor, ham_key_t *key, 
                    ham_record_t *record)
{
  ham_u32_t flags = 0;

  // if (m_config->direct_access && m_config->inmemory)
    // flags |= HAM_DIRECT_ACCESS;

#if 0
  if (!m_txn) {
    record->flags = HAM_RECORD_USER_ALLOC;
    record->data = m_useralloc;
  }
#endif

  return (ham_cursor_move((ham_cursor_t *)cursor, key, record,
                          HAM_CURSOR_NEXT | flags));
}

ham_status_t
HamsterDatabase::do_close_cursor(Cursor *cursor)
{
  return (ham_cursor_close((ham_cursor_t *)cursor));
}

void
HamsterDatabase::print_error(const char *function, ham_status_t st)
{
  ERROR(("%s failed with error %d (%s)\n", function, st, ham_strerror(st)));
}

