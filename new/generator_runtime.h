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

#ifndef RUNTIME_GENERATOR_H__
#define RUNTIME_GENERATOR_H__

#include <boost/random.hpp>
#include <boost/random/uniform_01.hpp>

#include "timer.h"
#include "generator.h"
#include "datasource.h"
#include "database.h"

//
// generates data based on configuration settings
//
class RuntimeGenerator : public Generator
{
    enum {
      kStateRunning = 0,
      kStateReopening,
      kStateStopped
    };

  public:
    // constructor
    RuntimeGenerator(Configuration *conf, Database *db);

    // destructor
    virtual ~RuntimeGenerator() {
      assert(m_txn == 0);
      assert(m_cursor == 0);
      delete m_datasource;
    }

    // executes the next command from the Datasource
    virtual bool execute();

    // opens the Environment; used for 'reopen'
    virtual void open();

    // closes the Environment; used for 'reopen'
    virtual void close();

  private:
    // creates the Environment
    void create();

    // inserts a key/value pair
    void insert();

    // erases a key/value pair
    void erase();

    // lookup of a key/value pair
    void find();

    // begins a new transaction
    void txn_begin();

    // commits a transaction
    void txn_commit();

    // aborts a transaction
    void txn_abort();

    // generates a new key, based on the Datasource
    ham_key_t generate_key();

    // generates a new record
    ham_record_t generate_record();

    // which command to execute next?
    int get_next_command();

    // returs true if test should stop now
    bool limit_reached();
 
    // the Database which executes the commands
    Database *m_db;

    // the current state (running, reopening etc)
    int m_state;

    // counting the number of operations
    uint64_t m_opcount;

    // counting inserted bytes
    uint64_t m_inserted_bytes;

    // the datasource
    Datasource *m_datasource;

    // a vector which temporarily stores the data from the Datasource
    std::vector<uint8_t> m_key_data;

    // a vector which temporarily stores the data for the records
    std::vector<uint8_t> m_record_data;

    // rng
    boost::mt19937 m_rng;

    // uniform distribution from 0..1
    boost::uniform_01<boost::mt19937> m_u01;

    // start time
    Timer<boost::chrono::system_clock> m_start;

    // the currently active Transaction
    Database::Transaction *m_txn;

    // the currently used Cursor
    Database::Cursor *m_cursor;
};

#endif /* RUNTIME_GENERATOR_H__ */

