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


#ifndef PARSER_HPP__
#define PARSER_HPP__

#include "config.hpp"

#include <string>
#include <vector>

struct config;
class Engine;

using namespace std;

class Parser
{
  public:
    Parser(config *c, const char *filename);

    bool process_line(unsigned lineno, Engine *engine);

    const std::string &get_line(unsigned lineno);

    unsigned get_max_lines() {
      return m_lines.size();
    }

  protected:
    char *get_token(char *line, unsigned *pos);

    char *strtok(char *s, char *t);

    vector<string> tokenize(const string &str);

  private:
    config *m_config;
    std::vector<std::string> m_lines;
};

#endif /* PARSER_HPP__ */
