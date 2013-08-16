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


#include "porting.hpp"
#include "config.hpp"
#include "os.hpp"
#include "berkeleydb.hpp"
#include "endian.hpp"
#include "misc.hpp"
#include "metrics.hpp"


static int
#if DB_VERSION_MAJOR == 5
compare_db(DB *db, const DBT *dbt1, const DBT *dbt2)
#else
compare_db(DB *db, const DBT *dbt1, const DBT *dbt2, size_t *)
#endif
{
  int l, r;
  memcpy(&l, dbt1->data, sizeof(l));
  memcpy(&r, dbt2->data, sizeof(r));
  l = ham_db2h32(l);
  r = ham_db2h32(r);

  if (l < r)
    return (-1);
  if (r < l)
    return (+1);
  return (0);
}

Berkeleydb::~Berkeleydb()
{
  close_txn();
  close_db();
  close_env();
}

ham_status_t 
Berkeleydb::create_env()
{
  timer t(this, timer::misc);

  int ret;

  ret = db_create(&m_db, 0, 0);
  if (ret)
    return (db2ham(ret));

  os::unlink(DB_PATH "test-berk.db");

  return (0);
}

ham_status_t 
Berkeleydb::create_db()
{
  timer t(this, timer::misc);

  int ret;

  if (m_config->is_numeric()) {
    ret = m_db->set_bt_compare(m_db, compare_db);
    if (ret)
      return (db2ham(ret));
  }

  if (m_config->duplicate) {
    ret = m_db->set_flags(m_db, DB_DUP);
    if (ret)
      return (db2ham(ret));
  }

  /* don't change dupe sorting - they're records and therefore never numeric!
   * if (m_config->sort_dupes && m_config->numeric) {
   *  ret = m_db->set_dup_compare(m_db, compare_db);
   *  if (ret)
   *    return (db2ham(ret));
   * }
   */

  ret = m_db->open(m_db, 0, m_config->inmemory ? 0 : DB_PATH "test-berk.db", 
      /* const char *dbname */ 0, DB_BTREE, DB_CREATE, 0644);
  if (ret)
    return (db2ham(ret));

  ret = m_db->cursor(m_db, 0, &m_cursor, 0);
  if (ret)
    return (db2ham(ret));

  return (0);
}

ham_status_t 
Berkeleydb::open_env()
{
  int ret;
  timer t(this, timer::misc);

  ret = db_create(&m_db, 0, 0);
  if (ret)
    return (db2ham(ret));

  if (m_config->is_numeric()) {
    ret = m_db->set_bt_compare(m_db, compare_db);
    if (ret)
      return (db2ham(ret));
  }

  if (m_config->duplicate) {
    ret = m_db->set_flags(m_db, DB_DUP);
    if (ret)
      return (db2ham(ret));
  }

  return (0);
}

ham_status_t 
Berkeleydb::open_db()
{
  timer t(this, timer::misc);

  int ret = m_db->open(m_db, 0, DB_PATH "test-berk.db", 0, DB_BTREE, 0, 0);
  if (ret)
    return (db2ham(ret));

  ret = m_db->cursor(m_db, 0, &m_cursor, 0);
  if (ret)
    return (db2ham(ret));

  return (0);
}

ham_status_t 
Berkeleydb::close_env()
{
  int ret;
  timer t(this, timer::misc);

  if (m_db) {
    ret = m_db->close(m_db, 0);
    if (ret)
      return (db2ham(ret));
    m_db = 0;
  }

  return (0);
}

ham_status_t 
Berkeleydb::close_txn()
{
  return 0;
}

ham_status_t 
Berkeleydb::close_db()
{
  int ret;
  timer t(this, timer::misc);

  if (m_cursor) {
    ret = m_cursor->c_close(m_cursor);
    if (ret)
      return (db2ham(ret));
    m_cursor = 0;
  }

  return (0);
}

ham_status_t 
Berkeleydb::flush()
{
  timer t(this, timer::misc);
  int ret = m_db->sync(m_db, 0);
  if (ret)
    return (db2ham(ret));
  return (0);
}

ham_status_t 
Berkeleydb::insert(ham_key_t *key, ham_record_t *record)
{
  int ret;
  DBT k, r;
  memset(&k, 0, sizeof(k));
  memset(&r, 0, sizeof(r));

  k.data = key->data;
  k.size = key->size;
  r.data = record->data;
  r.size = record->size;

  int flags = 0;
  if (!m_config->overwrite && !m_config->duplicate)
    flags |= DB_NOOVERWRITE;

  if (m_config->duplicate 
      && m_config->dupe_flags == HAM_DUPLICATE_INSERT_FIRST) {
    timer t(this, timer::cursor);
    ret = m_cursor->c_put(m_cursor, &k, &r, DB_KEYFIRST);
  }
  else if (0) { /* m_config->use_cursors || m_config->duplicate) {*/
    timer t(this, timer::cursor);
    ret = m_cursor->c_put(m_cursor, &k, &r, flags);
  }
  else {
    timer t(this, timer::insert);
    ret = m_db->put(m_db, 0, &k, &r, flags);
  }

  return (db2ham(ret));
}

ham_status_t 
Berkeleydb::erase(ham_key_t *key)
{
  int ret;
  DBT k, r;
  memset(&k, 0, sizeof(k));
  memset(&r, 0, sizeof(r));

  k.data = key->data;
  k.size = key->size;

  if (m_config->use_cursors) {
    timer t(this, timer::cursor);
    ret = m_cursor->c_get(m_cursor, &k, &r, DB_SET);
    if (ret)
      return (db2ham(ret));
    ret = m_cursor->c_del(m_cursor, 0);
  }
  else {
    timer t(this, timer::erase);
    ret = m_db->del(m_db, 0, &k, 0);
  }

  return (db2ham(ret));
}

ham_status_t 
Berkeleydb::find(ham_key_t *key, ham_record_t *record)
{
  int ret;
  DBT k, r;
  memset(&k, 0, sizeof(k));
  memset(&r, 0, sizeof(k));

  k.data = key->data;
  k.size = key->size;

  if (m_config->use_cursors) {
    timer t(this, timer::cursor);
    ret = m_cursor->c_get(m_cursor, &k, &r, DB_SET);
    if (ret)
      return (db2ham(ret));
  }
  else {
    timer t(this, timer::find);
    ret = m_db->get(m_db, 0, &k, &r, 0);
  }

  if (ret)
    return (db2ham(ret));

  record->data = r.data;
  record->size = r.size;
  return (0);
}

ham_status_t 
Berkeleydb::txn_begin()
{
  return (0);
}

ham_status_t 
Berkeleydb::txn_commit()
{
  return (0);
}

ham_status_t 
Berkeleydb::db2ham(int ret)
{
  switch (ret) {
    case 0: return (HAM_SUCCESS);
    case DB_KEYEXIST: return (HAM_DUPLICATE_KEY);
    case DB_NOTFOUND: return (HAM_KEY_NOT_FOUND);
  }

  TRACE(("unknown berkeley return code %d\n", ret));
  return ((ham_status_t)ret);
}

ham_status_t 
Berkeleydb::check_integrity(void)
{
  return 0;
}

void *
Berkeleydb::create_cursor(void)
{
  DBC *cursor;

  int ret = m_db->cursor(m_db, 0, &cursor, 0);
  if (ret) {
    TRACE(("failed to create cursor: %d\n", ret));
    exit(-1);
  }

  return ((void *)cursor);
}

ham_status_t 
Berkeleydb::get_previous(void *cursor, ham_key_t *key, 
          ham_record_t *record, int flags)
{
  DBT k, r;
  memset(&k, 0, sizeof(k));
  memset(&r, 0, sizeof(r));
  DBC *c = (DBC *)cursor;

  timer t(this, timer::cursor);

  if (flags == HAM_SKIP_DUPLICATES)
    flags = DB_PREV_NODUP;
  else
    flags = DB_PREV;

  int ret = c->c_get(c, &k, &r, flags);
  if (ret)
    return (db2ham(ret));
  key->data = k.data;
  key->size = k.size;
  record->data = r.data;
  record->size = r.size;
  return (0);
}

ham_status_t
Berkeleydb::get_next(void *cursor, ham_key_t *key, ham_record_t *record,
        int flags)
{
  DBT k, r;
  memset(&k, 0, sizeof(k));
  memset(&r, 0, sizeof(r));
  DBC *c = (DBC *)cursor;

  timer t(this, timer::cursor);

  if (flags == HAM_SKIP_DUPLICATES)
    flags = DB_NEXT_NODUP;
  else
    flags = DB_NEXT;

  int ret = c->c_get(c, &k, &r, flags);
  if (ret)
    return (db2ham(ret));
  key->data = k.data;
  key->size = k.size;
  record->data = r.data;
  record->size = r.size;
  return (0);
}

void 
Berkeleydb::close_cursor(void *cursor)
{
  DBC *c = (DBC *)cursor;

  int ret = c->c_close(c);
  if (ret) {
    TRACE(("failed to close cursor: %d\n", ret));
    exit(-1);
  }
}

void 
Berkeleydb::collect_metrics(void)
{
  database::collect_metrics();
}
