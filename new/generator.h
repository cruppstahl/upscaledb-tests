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

#include "metrics.h"
#include "database.h"

class Configuration;

//
// base class for generating test data
//
class Generator
{
  public:
    enum {
      kCommandInsert = 0,
      kCommandErase = 1,
      kCommandFind = 2,
      kCommandCommitTransaction = 3,
      kCommandCreate,
      kCommandOpen,
      kCommandClose,
      kCommandBeginTransaction,
      kCommandAbortTransaction,
      kCommandFlush,
      kCommandNop,
      kCommandFullcheck = 999999 // avoid conflicts with ham_status_t
    };

    // constructor
    Generator(int id, Configuration *conf, Database *db)
      : m_id(id), m_config(conf), m_db(db), m_last_status(0),
        m_opspersec_graph(0) {
      memset(&m_record, 0, sizeof(m_record));

      memset(&m_opspersec, 0, sizeof(m_opspersec));
      memset(&m_latency_graphs, 0, sizeof(m_latency_graphs));

      // only create graph output for the first hamsterdb thread!
      if (conf->metrics >= Configuration::kMetricsPng
           && !strcmp(db->get_name(), "hamsterdb")) {
        if (id == 0) {
          for (int i = 0; i < 4; i++) {
            char filename[128];
            sprintf(filename, "%s-lat%d.dat", m_db->get_name(), i);
            m_latency_graphs[i] = fopen(filename, "w");
            if (!m_latency_graphs[i]) {
              printf("error writing to file: %s\n", strerror(errno));
              exit(-1);
            }
            setvbuf(m_latency_graphs[i], NULL, _IOFBF, 10 * 1024 * 1024);
          }
          char filename[128];
          sprintf(filename, "%s-ops.dat", m_db->get_name());
          m_opspersec_graph = fopen(filename, "w");
          if (!m_opspersec_graph) {
            printf("error writing to file: %s\n", strerror(errno));
            exit(-1);
          }
          setvbuf(m_opspersec_graph, NULL, _IOFBF, 2 * 1024 * 1024);
        }
      }
    }

    // destructor
    virtual ~Generator() {
      for (int i = 0; i < 4; i++) {
        if (m_latency_graphs[i]) {
          fclose(m_latency_graphs[i]);
          m_latency_graphs[i] = 0;
        }
      }
      if (m_opspersec_graph) {
        fclose(m_opspersec_graph);
        m_opspersec_graph = 0;
      }
    }

    // Returns the database
    Database *get_db() {
      return (m_db);
    }
    
    // Returns the status of the last database operation
    ham_status_t get_status() const {
      return (m_last_status);
    }

    // executes the next generated statement; returns true if more statements
    // will follow, false when the test is done
    virtual bool execute() = 0;

    // opens the Environment; used for 'reopen'
    virtual void open() = 0;

    // closes the Environment; used for 'reopen'
    virtual void close() = 0;

    // returns true if the test was successful
    virtual bool was_successful() const = 0;

    // returns the collected metrics/statistics
    virtual void get_metrics(Metrics *metrics) = 0;

    // Returns the previously retrieved record
    const ham_record_t *get_record() const {
      return (&m_record);
    }

  protected:
    // adds information to the latency graph
    void add_latency_graph(int command, double time, double latency) {
      fprintf(m_latency_graphs[command], "%f %f\n", time, latency);
    }

    // add information to the "operations per second" graph
    void add_opspersec_graph(uint64_t elapsed_seconds) {
      fprintf(m_opspersec_graph, "%lu %lu %lu %lu %lu\n", elapsed_seconds,
              m_opspersec[0], m_opspersec[1], m_opspersec[2], m_opspersec[3]);
    }

    // unique ID - used to create the database
    int m_id;

    // the configuration settings
    Configuration *m_config;

    // the Database
    Database *m_db;

    // the status of the previous database operation
    ham_status_t m_last_status;

    // A record to store data from the last call to find() and
    // cursor_find(); used by the caller to compare results from
    // different databases
    ham_record_t m_record;

    // file handles for the latency output; index is the command id
    // (kCommandInsert, -Erase, -Find, -CommitTransaction)
    FILE *m_latency_graphs[4];

    // file handles for the operation-per-second
    FILE *m_opspersec_graph;

    // accumulating operations-per-seconds for the graphs
    uint64_t m_opspersec[4];
};

#endif /* GENERATOR_H__ */

