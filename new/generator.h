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


#ifndef GENERATOR_H__
#define GENERATOR_H__

class Configuration;
class Database;

//
// base class for generating test data
//
class Generator
{
  public:
    enum {
      kCommandCreate,
      kCommandOpen,
      kCommandClose,
      kCommandInsert,
      kCommandErase,
      kCommandFind,
      kCommandFullcheck,
      kCommandBeginTransaction,
      kCommandAbortTransaction,
      kCommandCommitTransaction
    };

    // constructor
    Generator(Configuration *conf)
      : m_conf(conf), m_last_status(0) {
    }

    // destructor
    virtual ~Generator() {
    }

    // executes the next generated statement; returns true if more statements
    // will follow, false when the test is done
    virtual bool execute() = 0;

    // opens the Environment; used for 'reopen'
    virtual void open() = 0;

    // closes the Environment; used for 'reopen'
    virtual void close() = 0;

  protected:
    // the configuration settings
    Configuration *m_conf;

    // the status of the previous database operation
    ham_status_t m_last_status;
};

#endif /* GENERATOR_H__ */

