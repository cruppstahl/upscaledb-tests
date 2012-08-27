
#include "porting.hpp"
#include "config.hpp"
#include "hamsterdb.hpp"
#include "endian.hpp"
#include "os.hpp"
#include "misc.hpp"

#include <boost/thread.hpp>
#include <ham/hamsterdb_int.h>

ham_env_t *Hamsterdb::ms_env;
boost::mutex Hamsterdb::ms_mutex;

static ham_u8_t aeskey[16]={ 
    0x00, 0x01, 0x02, 0x03, 
    0x04, 0x05, 0x06, 0x07, 
    0x08, 0x09, 0x0a, 0x0b, 
    0x0c, 0x0d, 0x0e, 0x0f
};

static int 
compare_keys(ham_db_t *db,
            const ham_u8_t *lhs, ham_size_t lhs_length, 
            const ham_u8_t *rhs, ham_size_t rhs_length)
{
    unsigned ulhs, urhs;

    (void)db;

    memcpy(&ulhs, lhs, sizeof(ulhs));
    memcpy(&urhs, rhs, sizeof(urhs));
    ulhs=ham_db2h32(ulhs);
    urhs=ham_db2h32(urhs);

    if (ulhs<urhs)
        return -1;
    if (ulhs==urhs)
        return 0;
    return 1;
}

Hamsterdb::~Hamsterdb()
{
    free(m_useralloc);

    if (m_db) {
        ham_delete(m_db);
        m_db=0;
    }

    if (ms_env) {
        ham_env_delete(ms_env);
        ms_env=0;
    }
}

ham_status_t 
Hamsterdb::create_env()
{
    ham_status_t st;
    ham_u32_t flags=0;
    ham_parameter_t params[6];

    boost::mutex::scoped_lock lock(ms_mutex);
    if (ms_env)
        return (0);

    if (!ms_env) {
        st=ham_env_new(&ms_env);
        if (st)
            return (st);
        ham_env_set_allocator(ms_env, m_mt);
    }

    params[0].name=HAM_PARAM_CACHESIZE;
    params[0].value=m_config->cachesize;
    params[1].name=HAM_PARAM_PAGESIZE;
    params[1].value=m_config->pagesize;
    params[2].name=HAM_PARAM_MAX_ENV_DATABASES;
    params[2].value=32;
    params[3].name=0;
    params[3].value=0;

    flags|=m_config->inmemory?HAM_IN_MEMORY_DB:0; 
    flags|=m_config->no_mmap?HAM_DISABLE_MMAP:0; 
    flags|=m_config->lock_excl?HAM_LOCK_EXCLUSIVE:0;
    flags|=m_config->duplicate?HAM_ENABLE_DUPLICATES:0;
    flags|=m_config->recovery?HAM_ENABLE_RECOVERY:0;
    flags|=m_config->cacheunlimited?HAM_CACHE_UNLIMITED:0;
    flags|=m_config->sort_dupes?HAM_SORT_DUPLICATES:0;
    flags|=m_config->enable_transactions?HAM_ENABLE_TRANSACTIONS:0;
    flags|=m_config->use_writethrough?HAM_WRITE_THROUGH:0;
    flags|=m_config->disable_async?HAM_DISABLE_ASYNCHRONOUS_FLUSH:0;

    os::unlink(DB_PATH "test-ham.db");

    timer t(this, timer::misc);

    /*
     * aes-encrypted databases are created in the environment
     *
     * !!
     * currently, all other parameters are ignored
     */
    st=ham_env_create_ex(ms_env, DB_PATH "test-ham.db", flags, 0664, 
            &params[0]);
    if (st)
        return (st);
    if (m_config->aes_encrypt) {
        st=ham_env_enable_encryption(ms_env, aeskey, 0);
        if (st)
            return (st);
    }

    return 0;
}

ham_status_t 
Hamsterdb::create_db()
{
    ham_status_t st;
    ham_parameter_t params[6];

    if (!m_db) {
        st=ham_new(&m_db);
        if (st)
            return (st);
    }

    timer t(this, timer::misc);

    params[0].name=HAM_PARAM_KEYSIZE;
    params[0].value=m_config->keysize;
    params[1].name=HAM_PARAM_DATA_ACCESS_MODE;
    params[1].value=m_config->data_access_mode;
    params[2].name=0;
    params[2].value=0;

    st=ham_env_create_db(ms_env, m_db, 1+m_id, 0, &params[0]);
    if (st) {
        TRACE(("failed to create database %d: %d\n", 1+m_id, st));
        return (st);
    }

    if (m_config->compression) {
        st=ham_enable_compression(m_db, 0, 0);
        if (st)
            return (st);
    }

    if (m_config->is_numeric()) {
        st=ham_set_compare_func(m_db, compare_keys);
        if (st)
            return (st);
    }

    return (ham_cursor_create(m_db, m_txn, 0, &m_cursor));
}

ham_status_t 
Hamsterdb::open_env()
{
    ham_status_t st;
    ham_u32_t flags=0;
    ham_parameter_t params[6];

    timer t(this, timer::misc);

    if (!ms_env) {
        st=ham_env_new(&ms_env);
        if (st)
            return (st);
        ham_env_set_allocator(ms_env, m_mt);
    }

    params[0].name=HAM_PARAM_CACHESIZE;
    params[0].value=m_config->cachesize;
    params[1].name=0;
    params[1].value=0;

    flags|=m_config->no_mmap?HAM_DISABLE_MMAP:0; 
    flags|=m_config->lock_excl?HAM_LOCK_EXCLUSIVE:0;
    flags|=m_config->cacheunlimited?HAM_CACHE_UNLIMITED:0;
    flags|=m_config->enable_transactions?HAM_ENABLE_TRANSACTIONS:0;
    flags|=m_config->use_writethrough?HAM_WRITE_THROUGH:0;
    flags|=m_config->disable_async?HAM_DISABLE_ASYNCHRONOUS_FLUSH:0;

    /*
     * aes encrypted databases are opened from an environment
     */
    st=ham_env_open_ex(ms_env, DB_PATH "test-ham.db", flags, &params[0]);
    if (st)
        return (st);
    if (m_config->aes_encrypt) {
        st=ham_env_enable_encryption(ms_env, aeskey, 0);
        if (st)
            return (st);
    }

    return (0);
}

ham_status_t 
Hamsterdb::open_db()
{
    ham_status_t st;
    ham_parameter_t params[6];

    timer t(this, timer::misc);

    if (!m_db) {
        st=ham_new(&m_db);
        if (st)
            return (st);
    }

    params[0].name=HAM_PARAM_DATA_ACCESS_MODE;
    params[0].value=m_config->data_access_mode;
    params[1].name=0;
    params[1].value=0;
    
    ham_u32_t flags=m_config->sort_dupes?HAM_SORT_DUPLICATES:0;
    st=ham_env_open_db(ms_env, m_db, 1+m_id, flags, &params[0]);
    if (st)
        return (st);

    if (m_config->compression) {
        st=ham_enable_compression(m_db, 0, 0);
        if (st)
            return (st);
    }

    if (m_config->is_numeric()) {
        st=ham_set_compare_func(m_db, compare_keys);
        if (st)
            return (st);
        if (m_config->sort_dupes) {
            st=ham_set_duplicate_compare_func(m_db, compare_keys);
            if (st)
                return (st);
        }
    }

    st=ham_cursor_create(m_db, m_txn, 0, &m_cursor);
    if (st)
        return (st);

    return (0);
}

ham_status_t 
Hamsterdb::close_txn()
{
    timer t(this, timer::misc);
    ham_status_t st=0;

    if (m_cursor) {
        st=ham_cursor_close(m_cursor);
        if (st)
            return (st);
        m_cursor=0;
    }

    if (m_txn) {
        st=ham_txn_commit(m_txn, 0);
        if (st)
            return (st);
        m_txn=0;
    }

    return (st);
}

ham_status_t 
Hamsterdb::close_db()
{
    timer t(this, timer::misc);
    ham_status_t st=0;

    if (m_db) {
        st=ham_close(m_db, HAM_AUTO_CLEANUP);
        if (st)
            return (st);
        ham_delete(m_db);
        m_db=0;
    }

    return (st);
}

ham_status_t 
Hamsterdb::close_env()
{
    timer t(this, timer::misc);
    ham_status_t st=0;

    if (ms_env)
        st=ham_env_close(ms_env, 0);
    return (st);
}

ham_status_t 
Hamsterdb::flush()
{
    timer t(this, timer::misc);

    return (ham_flush(m_db, 0));
}

ham_status_t 
Hamsterdb::insert(ham_key_t *key, ham_record_t *record)
{
    ham_u32_t flags=0;

    if (m_config->hints&HAM_HINT_RANDOM_ACCESS)
        flags|=HAM_HINT_RANDOM_ACCESS;
    if (m_config->hints&HAM_HINT_SEQUENTIAL)
        flags|=HAM_HINT_SEQUENTIAL;
    if (m_config->hints&HAM_HINT_UBER_FAST_ACCESS)
        flags|=HAM_HINT_UBER_FAST_ACCESS;

    if (m_config->use_cursors) {
        flags|=m_config->hints&HAM_HINT_APPEND;

        if (m_config->overwrite)
            flags|=HAM_OVERWRITE;
        else if (m_config->duplicate) {
            flags|=HAM_DUPLICATE;
            flags|=m_config->dupe_flags;
        }

        timer t(this, timer::cursor);
        return (ham_cursor_insert(m_cursor, key, record, flags));
    }
    else {
        if (m_config->overwrite)
            flags|=HAM_OVERWRITE;
        if (m_config->duplicate)
            flags|=HAM_DUPLICATE;

        timer t(this, timer::insert);
        return (ham_insert(m_db, m_txn, key, record, flags));
    }
}

ham_status_t 
Hamsterdb::erase(ham_key_t *key)
{
    ham_status_t st;
    ham_u32_t flags=0;

    if (m_config->hints&HAM_HINT_RANDOM_ACCESS)
        flags|=HAM_HINT_RANDOM_ACCESS;
    if (m_config->hints&HAM_HINT_SEQUENTIAL)
        flags|=HAM_HINT_SEQUENTIAL;
    if (m_config->hints&HAM_HINT_UBER_FAST_ACCESS)
        flags|=HAM_HINT_UBER_FAST_ACCESS;

    if (m_config->use_cursors) {
        timer t(this, timer::cursor);
        st=ham_cursor_find(m_cursor, key, flags);
        if (st)
            return (st);
        return (ham_cursor_erase(m_cursor, flags));
    }
    else {
        timer t(this, timer::erase);
        return (ham_erase(m_db, m_txn, key, flags));
    }
}

ham_status_t 
Hamsterdb::find(ham_key_t *key, ham_record_t *record)
{
    ham_status_t st;
    ham_u32_t flags=0;

    if (m_config->hints&HAM_HINT_RANDOM_ACCESS)
        flags|=HAM_HINT_RANDOM_ACCESS;
    if (m_config->hints&HAM_HINT_SEQUENTIAL)
        flags|=HAM_HINT_SEQUENTIAL;
    if (m_config->hints&HAM_HINT_UBER_FAST_ACCESS)
        flags|=HAM_HINT_UBER_FAST_ACCESS;

    if (m_config->direct_access && m_config->inmemory)
        flags|=HAM_DIRECT_ACCESS;

    if (!m_txn) {
        record->flags=HAM_RECORD_USER_ALLOC;
        record->data=m_useralloc;
    }

    if (m_config->use_cursors) {
        timer t(this, timer::cursor);
        st=ham_cursor_find(m_cursor, key, flags);
        if (st)
            return (st);
        return (ham_cursor_move(m_cursor, 0, record, flags));
    }
    else {
        timer t(this, timer::find);
        return (ham_find(m_db, m_txn, key, record, flags));
    }
}

ham_status_t 
Hamsterdb::txn_begin(void)
{
    ham_status_t st;

    assert(m_txn==0);

    if (m_cursor) {
        st=ham_cursor_close(m_cursor);
        if (st)
            return (st);
        m_cursor=0;
    }

    st=ham_txn_begin(&m_txn, ham_get_env(m_db), 0, 0, 0);
    if (st)
        return (st);

    st=ham_cursor_create(m_db, m_txn, 0, &m_cursor);
    if (st) {
        TRACE(("failed to create cursor: %d\n", st));
        exit(-1);
    }

    return (0);
}

ham_status_t 
Hamsterdb::txn_commit(void)
{
    ham_status_t st;

    assert(m_txn!=0);

    if (m_cursor) {
        st=ham_cursor_close(m_cursor);
        if (st)
            return (st);
        m_cursor=0;
    }

    st=ham_txn_commit(m_txn, 0);
    if (st)
        return (st);
    m_txn=0;
    return (0);
}

ham_status_t 
Hamsterdb::check_integrity(void)
{
	return ham_check_integrity(m_db, m_txn);
}

void *
Hamsterdb::create_cursor(void)
{
    ham_cursor_t *cursor;

    ham_status_t st=ham_cursor_create(m_db, m_txn, 0, &cursor);
    if (st) {
        TRACE(("failed to create cursor: %d\n", st));
        exit(-1);
    }

    return ((void *)cursor);
}

ham_status_t 
Hamsterdb::get_previous(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags)
{
    timer t(this, timer::cursor);

    if (m_config->direct_access && m_config->inmemory)
        flags|=HAM_DIRECT_ACCESS;

    if (!m_txn) {
        record->flags=HAM_RECORD_USER_ALLOC;
        record->data=m_useralloc;
    }

    return (ham_cursor_move((ham_cursor_t *)cursor, key, record, 
                    HAM_CURSOR_PREVIOUS|flags));
}

ham_status_t
Hamsterdb::get_next(void *cursor, ham_key_t *key, ham_record_t *record, 
                int flags)
{
    timer t(this, timer::cursor);

    if (m_config->direct_access && m_config->inmemory)
        flags|=HAM_DIRECT_ACCESS;

    if (!m_txn) {
        record->flags=HAM_RECORD_USER_ALLOC;
        record->data=m_useralloc;
    }

    return (ham_cursor_move((ham_cursor_t *)cursor, key, record, 
                    HAM_CURSOR_NEXT|flags));
}

void 
Hamsterdb::close_cursor(void *cursor)
{
    ham_status_t st=ham_cursor_close((ham_cursor_t *)cursor);
    if (st) {
        TRACE(("failed to close cursor: %d\n", st));
        exit(-1);
    }
}

void 
Hamsterdb::collect_metrics(void)
{
    database::collect_metrics();
    Metrics::get_instance()->add_metric(get_name(), "filesize", 
            os::get_filesize("test-ham.db"));
    Metrics::get_instance()->add_metric(get_name(), "mem-num-allocs", 
            m_mt->get_num_allocs());
    Metrics::get_instance()->add_metric(get_name(), "mem-peak-bytes",
            m_mt->get_peak_bytes());
    Metrics::get_instance()->add_metric(get_name(), "mem-total-bytes",
            m_mt->get_total_bytes());
}

