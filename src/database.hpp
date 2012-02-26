#ifndef DATABASE_HPP__
#define DATABASE_HPP__

#include <ham/hamsterdb.h>
#include <string.h>
#include "os.hpp"

struct config;

class Database
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

        timer(Database *db, Database::timer::idx store)
        :   m_db(db), m_store(store)
        {
            m_start=os::now();
        }

        ~timer()
        {
            m_db->m_profile[m_store]+=os::now()-m_start;
        }
    private:
        Database *m_db;
        Database::timer::idx m_store;
        ham_u64_t m_start;
    };

public:
    Database(int id, config *c)
    : m_id(id), m_config(c)
    {
        memset(m_profile, 0, sizeof(m_profile));
    }

    virtual ~Database(void)
    {
    }

    virtual ham_status_t create(void)=0;
    virtual ham_status_t open(void)=0;
    virtual ham_status_t close(void)=0;
    virtual ham_status_t flush(void)=0;
    virtual ham_status_t insert(ham_key_t *key, ham_record_t *record)=0;
    virtual ham_status_t erase(ham_key_t *key)=0;
    virtual ham_status_t find(ham_key_t *key, ham_record_t *record)=0;
    virtual ham_status_t txn_begin(void)=0;
    virtual ham_status_t txn_commit(void)=0;

    int get_id() { return m_id; }

    virtual ham_status_t check_integrity(void)=0;

	virtual void *create_cursor(void)=0;
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
