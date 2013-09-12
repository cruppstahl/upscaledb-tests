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

#ifndef DATABASE_HPP__
#define DATABASE_HPP__

#include <ham/hamsterdb.h>
#include <string.h>
#include "os.h"

class Configuration;

class Database
{
  protected:
    class timer
    {
      public:
        enum idx {
          kMisc = 0,
          kInsert,
          kErase,
          kFind,
          kTxn,
          kCursor
        };

        timer(Database *db, Database::timer::idx store)
          :   m_db(db), m_store(store) {
          m_start = Os::now();
        }

        ~timer() {
          m_db->m_profile[m_store] += Os::now() - m_start;
        }

      private:
        Database *m_db;
        Database::timer::idx m_store;
        ham_u64_t m_start;
    };

public:
    Database(int id, Configuration *c)
      : m_id(id), m_config(c) {
      memset(m_profile, 0, sizeof(m_profile));
    }

    virtual ~Database() {
    }

    virtual ham_status_t create_env() = 0;
    virtual ham_status_t create_db() = 0;
    virtual ham_status_t open_env() = 0;
    virtual ham_status_t open_db() = 0;
    virtual ham_status_t close_txn() = 0;
    virtual ham_status_t close_db() = 0;
    virtual ham_status_t close_env() = 0;
    virtual ham_status_t flush() = 0;
    virtual ham_status_t insert(ham_key_t *key, ham_record_t *record) = 0;
    virtual ham_status_t erase(ham_key_t *key) = 0;
    virtual ham_status_t find(ham_key_t *key, ham_record_t *record) = 0;
    virtual ham_status_t txn_begin() = 0;
    virtual ham_status_t txn_commit() = 0;

    int get_id() { return m_id; }
    virtual const char *get_name() = 0;

    virtual ham_status_t check_integrity() = 0;

	virtual void *create_cursor() = 0;
    virtual ham_status_t get_previous(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags) = 0;
    virtual ham_status_t get_next(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags) = 0;
    virtual void close_cursor(void *cursor) = 0;

    virtual void collect_metrics();

  protected:
    int m_id;
    Configuration *m_config;
    ham_u64_t m_profile[10];
    friend class timer;
};

#endif /* DATABASE_HPP__ */
