
#ifndef ENGINE_HPP__
#define ENGINE_HPP__

#include "database.hpp"

class parser;
struct config;

class engine
{
public:
    engine(config *c);
    ~engine();

    void set_parser(parser *p);

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
    parser *m_parser;
    unsigned m_opcount;
};

#endif /* ENGINE_HPP__ */
