#ifndef HAMSTERDB_HPP__
#define HAMSTERDB_HPP__

#include <ham/hamsterdb_int.h>

#include "database.hpp"

class hamsterdb : public database
{
public:
    hamsterdb(config *c)
    : database(c), m_db(0), m_env(0), m_cursor(0)
    {
    }

    ~hamsterdb(void);

    virtual ham_status_t create(void);
    virtual ham_status_t open(void);
    virtual ham_status_t close(void);
    virtual ham_status_t flush(void);
    virtual ham_status_t insert(ham_key_t *key, ham_record_t *record);
    virtual ham_status_t erase(ham_key_t *key);
    virtual ham_status_t find(ham_key_t *key, ham_record_t *record);

    virtual const char *get_name(void);

    virtual ham_status_t check_integrity(void);

    virtual void *create_cursor(void);
    virtual ham_status_t get_previous(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags);
    virtual ham_status_t get_next(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags);
    virtual void close_cursor(void *cursor);

protected:
    ham_db_t *m_db;
    ham_env_t *m_env;
    ham_cursor_t *m_cursor;
};

#endif /* HAMSTERDB_HPP__ */
