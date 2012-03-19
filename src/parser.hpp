
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
