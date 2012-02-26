#ifndef BERKELEYDB_HPP__
#define BERKELEYDB_HPP__

#include <db.h>
#include <ham/hamsterdb.h> // for ham_status_t, ham_key_t etc

#include "database.hpp"


class berkeleydb : public database
{
public:
    berkeleydb(config *c)
    : database(c), m_db(0), m_cursor(0)
    {
    }

    ~berkeleydb(void);

    virtual ham_status_t create(void);
    virtual ham_status_t open(void);
    virtual ham_status_t close(void);
    virtual ham_status_t flush(void);
    virtual ham_status_t insert(ham_key_t *key, ham_record_t *record);
    virtual ham_status_t erase(ham_key_t *key);
    virtual ham_status_t find(ham_key_t *key, ham_record_t *record);
    virtual ham_status_t txn_begin(void);
    virtual ham_status_t txn_commit(void);

    virtual const char *get_name(void);

    virtual ham_status_t check_integrity(void);

    virtual void *create_cursor(void);
    virtual ham_status_t get_previous(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags);
    virtual ham_status_t get_next(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags);
    virtual void close_cursor(void *cursor);
    virtual void print_metrics(void);

protected:
    ham_status_t db2ham(int ret);

    DB *m_db;
    DBC *m_cursor;
};

#endif /* BERKELEYDB_HPP__ */
