
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
    bool close_txn();
    bool close_env();
    bool flush();
    bool check_integrity();
    void *create_cursor();
    void close_cursor(void *cursor);

    int get_id() {
        return get_db()->get_id();
    }

    database *get_db() { 
        boost::mutex::scoped_lock lock(m_mutex);
        return m_db; 
    }

    ham_status_t get_status() {
        boost::mutex::scoped_lock lock(m_mutex);
        return m_status; 
    }

    const ham_record_t &get_record() {
        boost::mutex::scoped_lock lock(m_mutex);
        return m_record; 
    }

protected:
    bool owns_env() {
        return (dynamic_cast<Berkeleydb *>(m_db) || m_db->get_id()==1);
    }

    bool txn_begin();
    bool txn_commit();

    //bool compare_records(ham_record_t *rec1, ham_record_t *rec2);
    bool inc_opcount(void);

    database *m_db;
    config *m_config;
    Parser *m_parser;
    unsigned m_opcount;
    boost::mutex m_mutex;
    ham_status_t m_status;
    ham_record_t m_record;
    unsigned m_data_size;
    void *m_data_ptr;
};

#endif /* ENGINE_HPP__ */
