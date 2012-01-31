#ifndef HAMSTERDB_HPP__
#define HAMSTERDB_HPP__

#include <ham/hamsterdb_int.h>

#include "database.hpp"
#include "memtracker.hpp"

typedef ham_status_t (*dev_flush_func_t)(ham_device_t *);
typedef ham_status_t (*dev_read_func_t)(ham_device_t *, ham_offset_t, 
        void *, ham_offset_t);
typedef ham_status_t (*dev_write_func_t)(ham_device_t *, ham_offset_t, 
        void *, ham_offset_t);
typedef ham_status_t (*dev_read_page_func_t)(ham_device_t *, 
        struct ham_page_t *);
typedef ham_status_t (*dev_write_page_func_t)(ham_device_t *, 
        struct ham_page_t *);
typedef ham_status_t (*dev_alloc_func_t)(ham_device_t *, ham_size_t, 
            ham_offset_t *);
typedef ham_status_t (*dev_alloc_page_func_t)(ham_device_t *, 
        struct ham_page_t *);

class hamsterdb : public database
{
public:
    hamsterdb(config *c)
    : database(c), m_db(0), m_env(0), m_txn(0), m_cursor(0), m_mt(0),
        m_num_flushes(0), m_num_read(0), m_num_write(0),
        m_num_read_page(0), m_num_write_page(0), m_num_alloc(0),
        m_num_alloc_page(0)
    {
        m_mt=new TrackingAllocator();
    }

    ~hamsterdb(void);

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

    virtual void print_specific_profile(void);
    
protected:
    ham_db_t *m_db;
    ham_env_t *m_env;
    ham_txn_t *m_txn;
    ham_cursor_t *m_cursor;
    TrackingAllocator *m_mt;
    unsigned long m_num_flushes;
    unsigned long m_num_read;
    unsigned long m_num_write;
    unsigned long m_num_read_page;
    unsigned long m_num_write_page;
    unsigned long m_num_alloc;
    unsigned long m_num_alloc_page;

    void patch_device(void);

    static ham_status_t dev_new_flush(ham_device_t *);
    static ham_status_t dev_new_read(ham_device_t *, ham_offset_t, 
        void *, ham_offset_t);
    static ham_status_t dev_new_write(ham_device_t *, ham_offset_t, 
        void *, ham_offset_t);
    static ham_status_t dev_new_read_page(ham_device_t *, struct ham_page_t *);
    static ham_status_t dev_new_write_page(ham_device_t *, struct ham_page_t *);
    static ham_status_t dev_new_alloc_page(ham_device_t *, struct ham_page_t *);
    static ham_status_t dev_new_alloc(ham_device_t *, ham_size_t, 
            ham_offset_t *);

    dev_flush_func_t m_dev_old_flush;
    dev_read_func_t m_dev_old_read;
    dev_write_func_t m_dev_old_write;
    dev_read_page_func_t m_dev_old_read_page;
    dev_write_page_func_t m_dev_old_write_page;
    dev_alloc_func_t m_dev_old_alloc;
    dev_alloc_page_func_t m_dev_old_alloc_page;
};

#endif /* HAMSTERDB_HPP__ */
