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

#ifndef HAMSTERDB_HPP__
#define HAMSTERDB_HPP__

#include <boost/thread.hpp>
#include <ham/hamsterdb_int.h>

#include "database.hpp"

class Hamsterdb : public database
{
  public:
    Hamsterdb(int id, config *c)
      : database(id, c), m_db(0), m_txn(0), m_cursor(0) {
        m_useralloc = malloc(1024 * 1024 * 100);
    }

    ~Hamsterdb();

    virtual ham_status_t create_env();
    virtual ham_status_t create_db();
    virtual ham_status_t open_env();
    virtual ham_status_t open_db();
    virtual ham_status_t close_txn();
    virtual ham_status_t close_db();
    virtual ham_status_t close_env();
    virtual ham_status_t flush();
    virtual ham_status_t insert(ham_key_t *key, ham_record_t *record);
    virtual ham_status_t erase(ham_key_t *key);
    virtual ham_status_t find(ham_key_t *key, ham_record_t *record);
    virtual ham_status_t txn_begin();
    virtual ham_status_t txn_commit();

    virtual ham_status_t check_integrity();
    virtual const char *get_name() { return "hamsterdb"; }

    virtual void *create_cursor();
    virtual ham_status_t get_previous(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags);
    virtual ham_status_t get_next(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags);
    virtual void close_cursor(void *cursor);

    virtual void collect_metrics();

protected:
    static ham_env_t *ms_env;
    static boost::mutex ms_mutex;
    ham_db_t *m_db;
    ham_txn_t *m_txn;
    ham_cursor_t *m_cursor;
    void *m_useralloc;
};

#endif /* HAMSTERDB_HPP__ */
