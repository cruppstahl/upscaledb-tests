
#ifndef PARSER_HPP__
#define PARSER_HPP__

#include "config.hpp"

#include <string>
#include <vector>

class Engine;
struct config;

class Parser
{
public:
    Parser(config *c, Engine *e, const char *filename, config *cfg);

    bool process(void);

    bool process_line(char *line);

    unsigned get_lineno(void);

protected:
    char *get_token(char *line, unsigned *pos);

    char *strtok(char *s, char *t);

private:
    config *m_config;
    Engine *m_engine;
    unsigned m_lineno;
    std::vector<std::string> m_lines;
};

#endif /* PARSER_HPP__ */
