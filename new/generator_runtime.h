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
  public:
    // constructor
    RuntimeGenerator(Configuration *conf);

    // destructor
    virtual ~RuntimeGenerator() {
      delete m_datasource;
    }

    virtual bool execute(Database *db);

  private:
    int get_next_command();
    bool limit_reached();

    // counting the number of operations
    uint64_t m_opcount;

    // the datasource
    Datasource *m_datasource;

    // rng
    boost::mt19937 m_rng;

    // uniform distribution from 0..1
    boost::uniform_01<boost::mt19937> m_u01;

    // start time
    Timer<boost::chrono::system_clock> m_start;
};

#endif /* RUNTIME_GENERATOR_H__ */

