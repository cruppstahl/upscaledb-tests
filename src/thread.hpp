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


#ifndef THREAD_HPP__
#define THREAD_HPP__

#include <assert.h>

#include "controller.hpp"
#ifndef WIN32
#  include "config.h"
#endif
#include "parser.hpp"
#include "engine.hpp"

#include <boost/thread.hpp>

class Thread
{
  public:
    Thread(int id, Controller &ctrl, config &c, Parser &p, const char *backend)
      : m_fail(false), m_eof(false), m_running(false), m_controller(ctrl),
      m_lineno(0), m_engine(id, &c, &p, backend), m_parser(&p) {
      m_thread = boost::thread(boost::bind(&Thread::run, this));
    }

    int get_id() {
      return m_engine.get_id();
    }

    void join() {
      m_thread.join();
      assert(m_eof || m_lineno == 0);
      assert(!m_running);
    }

    unsigned get_lineno() {
      boost::mutex::scoped_lock lock(m_mutex);
      return get_lineno_nolock();
    }

    unsigned get_lineno_nolock() {
      return m_lineno;
    }

    bool success() {
      boost::mutex::scoped_lock lock(m_mutex);
      return !m_fail;
    }

    void wakeup() {
      boost::mutex::scoped_lock lock(m_mutex);
      m_running = true;
      m_cond.notify_one();
    }

    ham_status_t get_status() { return m_engine.get_status(); }

    database *get_db() { return m_engine.get_db(); }

    bool check_integrity() {
      if (!m_engine.check_integrity())
        exit(-1);
      return true;
    }

    void *create_cursor() { return m_engine.create_cursor(); }

    void close_cursor(void *c) { return m_engine.close_cursor(c); }

    const ham_record_t &get_record() { return m_engine.get_record(); }

    void flush() { m_engine.flush(); }

  private:
    void run() {
      while (get_lineno() < m_parser->get_max_lines()) {
        boost::mutex::scoped_lock lock(m_mutex);
        while (!m_running)
          m_cond.wait(lock);
        // printf("%d: line %d\n", m_engine.get_id(), m_lineno);

        bool ok = m_parser->process_line(m_lineno, &m_engine);
        m_lineno++;
        m_running = false;
        m_controller.wakeup();

        if (!ok) {
          m_fail = true;
          return;
        }
      }

      m_eof = true;
    }

    boost::mutex m_mutex;
    boost::condition m_cond;
    bool m_fail;
    bool m_eof;
    bool m_running;
    Controller &m_controller;
    unsigned m_lineno;
    Engine m_engine;
    Parser *m_parser;
    boost::thread m_thread;
};

#endif // THREAD_HPP__
