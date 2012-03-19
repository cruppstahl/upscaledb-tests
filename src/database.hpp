#ifndef DATABASE_HPP__
#define DATABASE_HPP__

#include <ham/hamsterdb.h>
#include <string.h>
#include "os.hpp"

struct config;

class database
{
protected:
    class timer
    {
    public:
        enum idx {
            misc=0,
            insert,
            erase,
            find,
            cursor
        };

        timer(database *db, database::timer::idx store)
        :   m_db(db), m_store(store)
        {
            m_start=os::now();
        }

        ~timer()
        {
            m_db->m_profile[m_store]+=os::now()-m_start;
        }
    private:
        database *m_db;
        database::timer::idx m_store;
        ham_u64_t m_start;
    };

public:
    database(int id, config *c)
      : m_id(id), m_config(c) {
        memset(m_profile, 0, sizeof(m_profile));
    }

    virtual ~database(void) {
    }

    virtual ham_status_t create_env()=0;
    virtual ham_status_t create_db()=0;
    virtual ham_status_t open_env()=0;
    virtual ham_status_t open_db()=0;
    virtual ham_status_t close_env()=0;
    virtual ham_status_t close_db()=0;
    virtual ham_status_t flush()=0;
    virtual ham_status_t insert(ham_key_t *key, ham_record_t *record)=0;
    virtual ham_status_t erase(ham_key_t *key)=0;
    virtual ham_status_t find(ham_key_t *key, ham_record_t *record)=0;
    virtual ham_status_t txn_begin()=0;
    virtual ham_status_t txn_commit()=0;

    int get_id() { return m_id; }

    virtual ham_status_t check_integrity()=0;

	virtual void *create_cursor()=0;
    virtual ham_status_t get_previous(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags)=0;
    virtual ham_status_t get_next(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags)=0;
    virtual void close_cursor(void *cursor)=0;

    virtual void collect_metrics();

protected:
    int m_id;
    config *m_config;
    ham_u64_t m_profile[10];
    friend class timer;
};

#endif /* DATABASE_HPP__ */
