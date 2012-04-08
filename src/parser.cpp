
#include "porting.hpp"
#include "parser.hpp"
#include "engine.hpp"
#include "misc.hpp"
#include "config.hpp"


Parser::Parser(config *c, const char *filename)
    :   m_config(c)
{
    FILE *f=stdin;
    if (filename)
        f=fopen(filename, "rt");

    if (!f) {
        TRACE(("failed to open %s\n", filename));
        exit(-1);
    }

    char line[1024*16];

    while (!feof(f)) {
        char *p=fgets(line, sizeof(line), f);
        if (!p)
            break;

        if (strstr(p, "CREATE")) {
            m_lines.push_back("CREATE_ENV");
            m_lines.push_back(p);
        }
        else if (strstr(p, "OPEN")) {
            m_lines.push_back("OPEN_ENV");
            m_lines.push_back(p);
        }
        else if (strstr(p, "CLOSE")) {
            m_lines.push_back(p);
            m_lines.push_back("CLOSE_ENV");
        }
        else
            m_lines.push_back(p);
    }

    if (f!=stdin)
        fclose(f);

    if (m_config->reopen) {
        m_lines.push_back("OPEN_ENV");
        m_lines.push_back("OPEN");
        m_lines.push_back("FULLCHECK");
        m_lines.push_back("CLOSE");
        m_lines.push_back("CLOSE_ENV");
    }
}

const std::string &
Parser::get_line(unsigned lineno)
{
    return m_lines[lineno];
}

bool
Parser::process_line(unsigned lineno, Engine *engine)
{
    unsigned pos=0;
    // create a local copy because the string will be modified
    std::string tmp=m_lines[lineno];
    char *line=(char *)tmp.c_str();
    vector<string> tokens=tokenize(m_lines[lineno]);
    if (tokens.empty())
        return 1;
    VERBOSE(("%d: line %u: reading token '%s' .......................\n", 
                engine->get_id(), lineno, tokens[0].c_str()));
    if (tokens[0]=="--") {
        return 1;
    }
    else if (tokens[0]=="CREATE_ENV") {
        return (engine->create_env());
    }
    else if (tokens[0]=="CREATE") {
        return (engine->create_db(strstr(line+pos, "NUMERIC_KEY") 
                    ? true : false));
    }
    else if (tokens[0]=="OPEN_ENV") {
        return (engine->open_env());
    }
    else if (tokens[0]=="OPEN") {
        return (engine->open_db(strstr(line+pos, "NUMERIC_KEY") 
                    ? true : false));
    }
    else if (tokens[0]=="BREAK") {
        printf("break at %s:%u\n", __FILE__, __LINE__);
        return 1;
    }
    else if (tokens[0]=="INSERT") {
        if (tokens.size()!=4) {
            TRACE(("line %d (INSERT): parser error\n", lineno+1));
            exit(-1);
        }

        const char *flags =tokens[1].c_str();
        const char *keytok=tokens[2].c_str();
        const char *data  =tokens[3].c_str();
        if (!data)
            data="";
        (void)flags;
        return (engine->insert(lineno, keytok, data));
    }
    else if (tokens[0]=="ERASE") {
        if (tokens.size()<3) {
            TRACE(("line %d (ERASE): parser error\n", lineno+1));
            exit(-1);
        }
        const char *flags =tokens[1].c_str();
        const char *keytok=tokens[2].c_str();
        (void)flags;
        return (engine->erase(keytok));
    }
    else if (tokens[0]=="FIND") {
        if (tokens.size()!=3) {
            TRACE(("line %d (FIND): parser error\n", lineno+1));
            exit(-1);
        }
        const char *flags =tokens[1].c_str();
        const char *keytok=tokens[2].c_str();
        (void)flags;
        return (engine->find(keytok));
    }
    else if (tokens[0]=="FULLCHECK") {
        return true; // this is executed in Controller.cpp
    }
    else if (tokens[0]=="CLOSE") {
        return (engine->close_db());
    }
    else if (tokens[0]=="CLOSE_ENV") {
        return (engine->close_env());
    }
    else if (tokens[0]=="FLUSH") {
        return (engine->flush());
    }

    TRACE(("line %d: invalid token '%s'\n", lineno, tokens[0].c_str()));
    return (false);
}

vector<string> 
Parser::tokenize(const string &str)
{
    vector<string> tokens;
    string delimiters = " \t\n\r()\",";
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
    return tokens;
}

