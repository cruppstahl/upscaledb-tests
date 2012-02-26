
#ifndef PARSER_HPP__
#define PARSER_HPP__

#include "config.hpp"

#include <stdio.h>

class Engine;
struct config;

class Parser
{
public:
    Parser(config *c, Engine *e, const char *filename, config *cfg);
    ~Parser(void);

public:
    bool process(void);

    bool process_line(char *line);

    unsigned get_lineno(void);

protected:
    char *get_token(char *line, unsigned *pos);

    char *strtok(char *s, char *t);

private:
    config *m_config;
    Engine *m_engine;
    FILE *m_f;
    unsigned m_lineno;
};

#endif /* PARSER_HPP__ */
