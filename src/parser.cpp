
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

        m_lines.push_back(p);
    }

    if (f!=stdin)
        fclose(f);
}

bool
Parser::process_line(unsigned lineno, Engine *engine)
{
    unsigned pos=0;
    char *line=(char *)m_lines[lineno].c_str();
    char *tok=get_token(line, &pos);
    if (!tok || tok[0]==0)
        return 1;
    VERBOSE(("%u: reading token '%s' .......................\n", 
                lineno, tok));
    if (strstr(tok, "--")) {
        return 1;
    }
    else if (!strcasecmp(tok, "CREATE")) {
        return (engine->create(strstr(line+pos, "NUMERIC_KEY") 
                    ? true : false));
    }
    else if (!strcasecmp(tok, "OPEN")) {
        return (engine->open(strstr(line+pos, "NUMERIC_KEY") 
                    ? true : false));
    }
    else if (!strcasecmp(tok, "BREAK")) {
        printf("break at %s:%u\n", __FILE__, __LINE__);
        return 1;
    }
    else if (!strcasecmp(tok, "INSERT")) {
        const char *flags =strtok(&line[pos], (char *)",");
        const char *keytok=strtok(0, (char *)",");
        const char *data  =strtok(0, (char *)",");
        if (!data)
            data="";
        (void)flags;
        return (engine->insert(lineno, keytok, data));
    }
    else if (!strcasecmp(tok, "ERASE")) {
        const char *flags =strtok(&line[pos], (char *)",");
        const char *keytok=strtok(0, (char *)",");
        (void)flags;
        return (engine->erase(keytok));
    }
    else if (!strcasecmp(tok, "FIND")) {
        const char *flags =strtok(&line[pos], (char *)",");
        const char *keytok=strtok(0, (char *)",");
        (void)flags;
        return (engine->find(keytok));
    }
    else if (!strcasecmp(tok, "FULLCHECK")) {
        return true; // TODO return (engine->fullcheck());
    }
    else if (!strcasecmp(tok, "CLOSE")) {
        return (engine->close());
    }
    else if (!strcasecmp(tok, "FLUSH")) {
        return (engine->flush());
    }

    TRACE(("line %d: invalid token '%s'\n", lineno, tok));
    return (false);
}

char *
Parser::get_token(char *line, unsigned *pos)
{
    char *r=&line[*pos];
    while (*r && (*r==' ' || *r=='\t' || *r=='\r' || *r=='\n'))
        r++;
    line=r;
    while (*line && *line!=' ' && *line!='\t' && *line!='\r' && *line!='\n')
        line++;
    *line=0;
    *pos=(unsigned)(line-r+1);

    return (r);
}

char *
Parser::strtok(char *s, char *t)
{
    char *e, *p=::strtok(s, t);
    if (!p) {
        TRACE(("expected token '%s'\n", t));
        exit(-1);
    }
    while (*p==' ' || *p=='\t' || *p=='\n' || *p=='\r' || *p=='(' || *p=='"')
        p++;
    e=p+strlen(p)-1;
    while (*e==' ' || *e=='\t' || *e=='\n' || *e=='\r' || *e==')' || *e=='"')
        e--;
    *(e+1)=0;
    return p;
}

