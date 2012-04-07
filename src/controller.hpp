
#ifndef CONTROLLER_HPP__
#define CONTROLLER_HPP__

#include <boost/thread/condition.hpp>
#include <iostream>

#include "parser.hpp"
#include "config.hpp"

class Thread;

class Controller
{
  public:
    Controller(config *config, Parser &parser) 
      : m_lineno(0), m_config(config), m_parser(parser) {
    }

    void wakeup() {
        m_controller_cond.notify_one();
    }

    void run(std::vector<Thread *> &threads);

    bool reached_eof() {
        return m_lineno >= m_parser.get_max_lines();
    }

    bool reached_line(std::vector<Thread *> &threads, unsigned line);

    unsigned next();

    bool has_failure(std::vector<Thread *> &threads);

  private:
    void compare_status(std::vector<Thread *> &threads);
    void compare_records(std::vector<Thread *> &threads);
    void compare_fullcheck(std::vector<Thread *> &threads);

    boost::condition m_controller_cond;
    boost::mutex m_mutex;
    unsigned m_lineno;
    config *m_config;
    Parser &m_parser;
};

#endif /* CONTROLLER_HPP__ */
