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

#include <iostream>
#include <string>

#include "porting.hpp"
#include "config.hpp"
#include "endian.hpp"
#include "engine.hpp"
#include "parser.hpp"
#include "hamsterdb.hpp"
#include "berkeleydb.hpp"
#include "misc.hpp"


Engine::Engine(int id, config *c, Parser *p, const std::string &name)
  : m_config(c), m_parser(p), m_opcount(0), m_status(0),
    m_data_size(0), m_data_ptr(0)
{
  boost::mutex::scoped_lock lock(m_mutex);
  memset(&m_record, 0, sizeof(m_record));
  if (name == "hamsterdb")
    m_db = new Hamsterdb(id, m_config);
  else
    m_db = new Berkeleydb(id, m_config);
}

Engine::~Engine()
{
  boost::mutex::scoped_lock lock(m_mutex);
  if (m_data_ptr)
    free(m_data_ptr);
  if (m_db) {
    m_db->close_txn();
    m_db->close_db();
    m_db->close_env();
    delete m_db;
    m_db = 0;
  }
}

bool 
Engine::create_env()
{
  boost::mutex::scoped_lock lock(m_mutex);

  // the Environment is only created by thread #1
  if (!owns_env())
    return true;

  m_status = m_db->create_env();
  if (m_status) {
    TRACE(("db: create_env failed w/ status %d\n", m_status));
    return (false);
  }

  return (true);
}

bool 
Engine::create_db(bool numeric)
{
  boost::mutex::scoped_lock lock(m_mutex);

  if (numeric)
    m_config->set_numeric(true);

  m_status = m_db->create_db();
  if (m_status) {
    TRACE(("db: create_db failed w/ status %d\n", m_status));
    return (false);
  }

  if (m_config->txn_group)
    return (txn_begin());

  return (true);
}

bool 
Engine::open_env()
{
  boost::mutex::scoped_lock lock(m_mutex);

  // the Environment is only created by thread #1
  if (!owns_env())
    return true;

  m_status = m_db->open_env();
  if (m_status) {
    TRACE(("db: open_env failed w/ status %d\n", m_status));
    return (false);
  }

  return (true);
}

bool 
Engine::open_db(bool numeric)
{
  boost::mutex::scoped_lock lock(m_mutex);

  if (numeric)
    m_config->set_numeric(true);

  m_status = m_db->open_db();
  if (m_status) {
    TRACE(("db: open_db failed w/ status %d\n", m_status));
    return (false);
  }

  return (true);
}

bool 
Engine::insert(unsigned lineno, const char *keytok, const char *data)
{
  boost::mutex::scoped_lock lock(m_mutex);
  ham_u32_t numkey=0;
  ham_size_t data_size;
  ham_key_t key;
  ham_record_t rec;

  VERBOSE(("insert: key: %s, data: %s\n", keytok, data));

  memset(&key, 0, sizeof(key));
  memset(&rec, 0, sizeof(rec));

  /* check flag NUMERIC_KEY */
  if (m_config->is_numeric()) {
    numkey = strtoul(keytok, 0, 0);
    if (!numkey) {
      TRACE(("key is invalid\n"));
      return (false);
    }
    numkey = ham_h2db32(numkey);
    key.data = (void *)&numkey;
    key.size = sizeof(numkey);
  }
  else {
    key.data = (void *)keytok;
    key.size = (ham_size_t)strlen(keytok) + 1;
  }

  /* allocate and initialize data */
  data_size = strtoul(data, 0, 0);

  if (data_size) {
    if (data_size > m_data_size) {
      m_data_size = data_size;
      m_data_ptr = realloc(m_data_ptr, data_size);
    }
    /* always start with a random number - otherwise berkeleydb fails
     * too often when duplicate keys are inserted with duplicate
     * records */
    for (ham_size_t i = 0; i < data_size; i++)
      ((char *)m_data_ptr)[i] = (lineno + i) & 0xff;
    if (data_size >= sizeof(unsigned))
      *(unsigned *)m_data_ptr = lineno;

    rec.data = m_data_ptr;
    rec.size = data_size;
  }

  m_status = m_db->insert(&key, &rec);
  if (m_status)
    VERBOSE(("db: insert failed w/ status %d\n", m_status));

  if (m_config->txn_group > 0)
    return (inc_opcount());
  return (true);
}

bool 
Engine::erase(const char *keytok)
{
  boost::mutex::scoped_lock lock(m_mutex);
  ham_u32_t numkey = 0;
  ham_key_t key;

  VERBOSE(("erase: key: %s\n", keytok));

  memset(&key, 0, sizeof(key));

  /*
   * check flag NUMERIC_KEY
   */
  if (m_config->is_numeric()) {
    numkey = strtoul(keytok, 0, 0);
    if (!numkey) {
      TRACE(("key is invalid\n"));
      return (false);
    }
    numkey = ham_h2db32(numkey);
    key.data = (void *)&numkey;
    key.size = sizeof(numkey);
  }
  else {
    key.data = (void *)keytok;
    key.size = (ham_size_t)strlen(keytok);
  }

  m_status = m_db->erase(&key);
  if (m_status)
    VERBOSE(("db: erase failed w/ status %d\n", m_status));

  if (m_config->txn_group > 0)
    return (inc_opcount());

  return (true);
}

bool 
Engine::find(const char *keytok)
{
  boost::mutex::scoped_lock lock(m_mutex);
  ham_u32_t numkey = 0;
  ham_key_t key;

  VERBOSE(("find: key: %s\n", keytok));

  memset(&key, 0, sizeof(key));
  memset(&m_record, 0, sizeof(m_record));

  /*
   * check flag NUMERIC_KEY
   */
  if (m_config->is_numeric()) {
    numkey = strtoul(keytok, 0, 0);
    if (!numkey) {
      TRACE(("key is invalid\n"));
      return (false);
    }
    numkey = ham_h2db32(numkey);
    key.data = (void *)&numkey;
    key.size = sizeof(numkey);
  }
  else {
    key.data = (void *)keytok;
    key.size = (ham_size_t)strlen(keytok);
  }

  m_status = m_db->find(&key, &m_record);
  if (m_status)
    VERBOSE(("db: find failed w/ status %d\n", m_status));

  if (m_config->txn_group > 0)
    return (inc_opcount());

  return (true);
}

bool 
Engine::close_txn()
{
  boost::mutex::scoped_lock lock(m_mutex);

  m_status = m_db->close_txn();
  if (m_status) {
    TRACE(("db: close_txn failed w/ status %d\n", m_status));
    return (false);
  }
  return (true);
}

bool 
Engine::close_db()
{
  boost::mutex::scoped_lock lock(m_mutex);

  m_status = m_db->close_db();
  if (m_status) {
    TRACE(("db: close_db failed w/ status %d\n", m_status));
    return (false);
  }

  // collect metrics before the memory allocator is deleted in ham_env_close
  m_db->collect_metrics();

  return (true);
}

bool 
Engine::close_env()
{
  boost::mutex::scoped_lock lock(m_mutex);

  // the Environment is only closed by thread #1
  if (!owns_env())
    return true;

  m_status = m_db->close_env();
  if (m_status) {
    TRACE(("db: close failed w/ status %d\n", m_status));
    return (false);
  }

  return (true);
}

bool 
Engine::flush()
{
  boost::mutex::scoped_lock lock(m_mutex);
  m_status = m_db->flush();
  if (m_status) {
    TRACE(("db: flush failed w/ status %d\n", m_status));
    return (false);
  }

  return (true);
}

bool 
Engine::txn_begin()
{
  m_status = m_db->txn_begin();
  if (m_status) {
    TRACE(("db: txn_begin failed w/ status %d\n", m_status));
    return (false);
  }

  return (true);
}

bool 
Engine::txn_commit()
{
  m_status = m_db->txn_commit();
  if (m_status) {
    TRACE(("db: txn_begin failed w/ status %d\n", m_status));
    return (false);
  }

  return (true);
}

bool 
Engine::check_integrity()
{
  boost::mutex::scoped_lock lock(m_mutex);

  m_status = m_db->check_integrity();
  if (m_status) {
    TRACE(("db: check_integrity failed w/ status %d\n", m_status));
    return (false);
  }

  return (true);
}

void * 
Engine::create_cursor()
{
  boost::mutex::scoped_lock lock(m_mutex);

  return m_db->create_cursor();
}

void
Engine::close_cursor(void *cursor)
{
  boost::mutex::scoped_lock lock(m_mutex);

  return m_db->close_cursor(cursor);
}

bool 
Engine::inc_opcount()
{
  if (++m_opcount >= m_config->txn_group) {
    if (!txn_commit())
      return (false);
    if (!txn_begin())
      return (false);
    m_opcount=0;
  }

  return (true);
}

