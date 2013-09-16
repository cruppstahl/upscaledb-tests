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
    RuntimeGenerator(Configuration *conf);

    // destructor
    virtual ~RuntimeGenerator() {
      assert(!m_txn_is_active);
      delete m_datasource;
    }

    // executes the next command from the Datasource
    virtual bool execute(Database *db);

    // opens the Environment; used for 'reopen'
    virtual void open(Database *db);

    // closes the Environment; used for 'reopen'
    virtual void close(Database *db);

  private:
    int get_next_command();
    bool limit_reached();

    // the current state (running, reopening etc)
    int m_state;

    // counting the number of operations
    uint64_t m_opcount;

    // counting inserted bytes
    uint64_t m_inserted_bytes;

    // the datasource
    Datasource *m_datasource;

    // rng
    boost::mt19937 m_rng;

    // uniform distribution from 0..1
    boost::uniform_01<boost::mt19937> m_u01;

    // start time
    Timer<boost::chrono::system_clock> m_start;

    // is a Transaction currently active?
    bool m_txn_is_active;
};

#endif /* RUNTIME_GENERATOR_H__ */

