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


#ifndef ENGINE_HPP__
#define ENGINE_HPP__

#include <boost/thread.hpp>
#include <string>

#include "database.h"
#include "berkeleydb.h"

class Parser;
class Configuration;

class Engine
{
  public:
    Engine(int id, Configuration *c, Parser *p, const std::string &name);
    ~Engine();

    bool create_env();
    bool create_db(bool numeric);
    bool open_env();
    bool open_db(bool numeric);
    bool insert(unsigned lineno, const char *keytok, const char *data);
    bool erase(const char *keytok);
    bool find(const char *keytok);
    bool fullcheck(void);
    bool close_db();
    bool close_txn();
    bool close_env();
    bool flush();
    bool check_integrity();
    void *create_cursor();
    void close_cursor(void *cursor);

    int get_id() {
      return get_db()->get_id();
    }

    Database *get_db() { 
      boost::mutex::scoped_lock lock(m_mutex);
      return m_db; 
    }

    ham_status_t get_status() {
      boost::mutex::scoped_lock lock(m_mutex);
      return m_status; 
    }

    const ham_record_t &get_record() {
      boost::mutex::scoped_lock lock(m_mutex);
      return m_record; 
    }

  protected:
    bool owns_env() {
      return (dynamic_cast<Berkeleydb *>(m_db) || m_db->get_id() == 1);
    }

    bool txn_begin();
    bool txn_commit();

    //bool compare_records(ham_record_t *rec1, ham_record_t *rec2);
    bool inc_opcount();

    Database *m_db;
    Configuration *m_config;
    Parser *m_parser;
    unsigned m_opcount;
    boost::mutex m_mutex;
    ham_status_t m_status;
    ham_record_t m_record;
    unsigned m_data_size;
    void *m_data_ptr;
};

#endif /* ENGINE_HPP__ */
