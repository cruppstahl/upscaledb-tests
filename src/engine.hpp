
#ifndef ENGINE_HPP__
#define ENGINE_HPP__

#include <boost/thread.hpp>
#include <string>

#include "database.hpp"
#include "berkeleydb.hpp"

class Parser;
struct config;

class Engine
{
public:
    Engine(int id, config *c, Parser *p, const std::string &name);
    ~Engine();

    bool create_env();
    bool create_db(bool numeric);
    bool open_env();
    bool open_db(bool numeric);
    bool insert(unsigned lineno, const char *keytok, const char *data);
    bool erase(const char *keytok);
    bool find(const char *keytok);
    bool fullcheck(void);
    bool close_db();
    bool close_env();
    bool flush(void);
    bool txn_begin(void);
    bool txn_commit(void);

    int get_id() {
        return get_db()->get_id();
    }

    database *get_db() { 
        boost::mutex::scoped_lock lock(m_mutex);
        return m_db; 
    }

protected:
    bool owns_env() {
        return (dynamic_cast<Berkeleydb *>(m_db) || m_db->get_id()!=1);
    }

    //bool compare_records(ham_record_t *rec1, ham_record_t *rec2);
    bool inc_opcount(void);

    //bool compare_status(ham_status_t st[2]);

    database *m_db;
    config *m_config;
    Parser *m_parser;
    unsigned m_opcount;
    boost::mutex m_mutex;
};

#endif /* ENGINE_HPP__ */
