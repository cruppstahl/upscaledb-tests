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

#ifndef HAMSTERDB_H__
#define HAMSTERDB_H__

#include <ham/hamsterdb.h>
#include <ham/hamsterdb_srv.h>

#include "mutex.h"
#include "database.h"

//
// abstract base class wrapping a database backend (i.e. hamsterdb,
// berkeleydb)
//
class HamsterDatabase : public Database
{
  public:
    HamsterDatabase(int id, Configuration *config)
      : Database(id, config), m_db(0) {
    }

    // Returns a descriptive name
    virtual const char *get_name() const {
      return ("hamsterdb");
    }

  protected:
    // the actual implementation(s)
    virtual ham_status_t do_create_env();
    virtual ham_status_t do_open_env();
    virtual ham_status_t do_close_env();
    virtual ham_status_t do_create_db();
    virtual ham_status_t do_open_db();
    virtual ham_status_t do_close_db();
    virtual ham_status_t do_flush();
    virtual ham_status_t do_insert(Transaction *txn, ham_key_t *key,
                    ham_record_t *record);
    virtual ham_status_t do_erase(Transaction *txn, ham_key_t *key);
    virtual ham_status_t do_find(Transaction *txn, ham_key_t *key,
                    ham_record_t *record);
    virtual ham_status_t do_check_integrity(Transaction *txn);

    virtual Transaction *do_txn_begin();
    virtual ham_status_t do_txn_commit(Transaction *txn);
    virtual ham_status_t do_txn_abort(Transaction *txn);

	virtual Cursor *do_cursor_create(Transaction *txn);
    virtual ham_status_t do_cursor_insert(Cursor *cursor, ham_key_t *key,
                    ham_record_t *record);
    virtual ham_status_t do_cursor_erase(Cursor *cursor, ham_key_t *key);
    virtual ham_status_t do_cursor_find(Cursor *cursor, ham_key_t *key,
                    ham_record_t *record);
    virtual ham_status_t do_cursor_get_previous(Cursor *cursor, ham_key_t *key, 
                    ham_record_t *record);
    virtual ham_status_t do_cursor_get_next(Cursor *cursor, ham_key_t *key, 
                    ham_record_t *record);
    virtual ham_status_t do_cursor_close(Cursor *cursor);

  private:
    static Mutex ms_mutex;
    static ham_env_t *ms_env;
    static ham_env_t *ms_remote_env;
    static ham_srv_t *ms_srv;

    ham_db_t *m_db;
};

#endif /* HAMSTERDB_H__ */
