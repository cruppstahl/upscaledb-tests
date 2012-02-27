#ifndef HAMSTERDB_HPP__
#define HAMSTERDB_HPP__

#include <ham/hamsterdb_int.h>

#include "database.hpp"
#include "memtracker.hpp"

class Hamsterdb : public database
{
public:
    Hamsterdb(int id, config *c)
    : database(id, c), m_db(0), m_env(0), m_txn(0), m_cursor(0), m_mt(0)
    {
        m_mt=new TrackingAllocator();
    }

    ~Hamsterdb(void);

    virtual ham_status_t create(void);
    virtual ham_status_t open(void);
    virtual ham_status_t close(void);
    virtual ham_status_t flush(void);
    virtual ham_status_t insert(ham_key_t *key, ham_record_t *record);
    virtual ham_status_t erase(ham_key_t *key);
    virtual ham_status_t find(ham_key_t *key, ham_record_t *record);
    virtual ham_status_t txn_begin(void);
    virtual ham_status_t txn_commit(void);

    virtual ham_status_t check_integrity(void);

    virtual void *create_cursor(void);
    virtual ham_status_t get_previous(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags);
    virtual ham_status_t get_next(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags);
    virtual void close_cursor(void *cursor);

    virtual void collect_metrics(void);

protected:
    ham_db_t *m_db;
    ham_env_t *m_env;
    ham_txn_t *m_txn;
    ham_cursor_t *m_cursor;
    TrackingAllocator *m_mt;
};

#endif /* HAMSTERDB_HPP__ */
