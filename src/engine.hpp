
#ifndef ENGINE_HPP__
#define ENGINE_HPP__

#include "database.hpp"

class Parser;
struct config;

class Engine
{
public:
    Engine(config *c);
    ~Engine();

    void set_parser(Parser *p);

    bool create(bool numeric);
    bool open(bool numeric);
    bool insert(const char *keytok, const char *data);
    bool erase(const char *keytok);
    bool find(const char *keytok);
    bool fullcheck(void);
    bool close(bool noreopen=false);
    bool flush(void);
    bool txn_begin(void);
    bool txn_commit(void);

    database *get_db(int i) { return m_db[i]; }

protected:
    bool compare_records(ham_record_t *rec1, ham_record_t *rec2);
    bool inc_opcount(void);

    bool compare_status(ham_status_t st[2]);

    database *m_db[2];
    config *m_config;
    Parser *m_parser;
    unsigned m_opcount;
};

#endif /* ENGINE_HPP__ */
