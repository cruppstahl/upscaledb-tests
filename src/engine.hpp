
#ifndef ENGINE_HPP__
#define ENGINE_HPP__

#include <string>

#include "database.hpp"

class Parser;
struct config;

class Engine
{
public:
    Engine(config *c, Parser *p, const std::string &name);
    ~Engine();

    bool create(bool numeric);
    bool open(bool numeric);
    bool insert(unsigned lineno, const char *keytok, const char *data);
    bool erase(const char *keytok);
    bool find(const char *keytok);
    bool fullcheck(void);
    bool close(bool noreopen=false);
    bool flush(void);
    bool txn_begin(void);
    bool txn_commit(void);

    database *get_db() { return m_db; }

protected:
    //bool compare_records(ham_record_t *rec1, ham_record_t *rec2);
    bool inc_opcount(void);

    //bool compare_status(ham_status_t st[2]);

    database *m_db;
    config *m_config;
    Parser *m_parser;
    unsigned m_opcount;
};

#endif /* ENGINE_HPP__ */
