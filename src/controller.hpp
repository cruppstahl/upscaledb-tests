
#ifndef CONTROLLER_HPP__
#define CONTROLLER_HPP__

#include <boost/thread/condition.hpp>
#include <iostream>

#include "parser.hpp"

class Thread;

class Controller
{
  public:
    Controller(Parser &parser) 
      : m_lineno(0), m_parser(parser) {
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
    boost::condition m_controller_cond;
    boost::mutex m_mutex;
    unsigned m_lineno;
    Parser &m_parser;
};

#endif /* CONTROLLER_HPP__ */
