
#include "porting.hpp"
#include "config.hpp"
#include "hamsterdb.hpp"
#include "endian.hpp"
#include "os.hpp"
#include "misc.hpp"



static ham_u8_t aeskey[16]={ 
    0x00, 0x01, 0x02, 0x03, 
    0x04, 0x05, 0x06, 0x07, 
    0x08, 0x09, 0x0a, 0x0b, 
    0x0c, 0x0d, 0x0e, 0x0f
};

static int 
my_compare_keys(ham_db_t *db,
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

hamsterdb::~hamsterdb()
{
    if (m_db) {
        ham_delete(m_db);
        m_db=0;
    }

    if (m_env) {
        ham_env_delete(m_env);
        m_env=0;
    }

    print_profile();
}

ham_status_t 
hamsterdb::create()
{
    ham_status_t st;
    ham_u32_t flags=0;
    ham_parameter_t params[6];

    if (!m_env) {
        st=ham_env_new(&m_env);
        if (st)
            return (st);
    }

    if (!m_db) {
        st=ham_new(&m_db);
        if (st)
            return (st);
    }

    params[0].name=HAM_PARAM_CACHESIZE;
    params[0].value=m_config->cachesize;
    params[1].name=HAM_PARAM_KEYSIZE;
    params[1].value=m_config->keysize;
    params[2].name=HAM_PARAM_PAGESIZE;
    params[2].value=m_config->pagesize;
    params[3].name=HAM_PARAM_DATA_ACCESS_MODE;
    params[3].value=m_config->data_access_mode;
    params[4].name=0;
    params[4].value=0;

    flags|=m_config->inmemory?HAM_IN_MEMORY_DB:0; 
    flags|=m_config->no_mmap?HAM_DISABLE_MMAP:0; 
    flags|=m_config->lock_excl?HAM_LOCK_EXCLUSIVE:0;
    flags|=m_config->duplicate?HAM_ENABLE_DUPLICATES:0;
    flags|=m_config->recovery?HAM_ENABLE_RECOVERY:0;
    flags|=m_config->cacheunlimited?HAM_CACHE_UNLIMITED:0;
    flags|=m_config->sort_dupes?HAM_SORT_DUPLICATES:0;
    flags|=m_config->enable_transactions?HAM_ENABLE_TRANSACTIONS:0;
    flags|=m_config->use_writethrough?HAM_WRITE_THROUGH:0;

    os::unlink(DB_PATH "test-ham.db");

    timer t(this, timer::misc);

    /*
     * aes-encrypted databases are created in the environment
     *
     * !!
     * currently, all other parameters are ignored
     */
    if (m_config->aes_encrypt) {
        st=ham_env_create_ex(m_env, DB_PATH "test-ham.db", flags, 0664, 0);
        if (st)
            return (st);
        st=ham_env_enable_encryption(m_env, aeskey, 0);
        if (st)
            return (st);
        st=ham_env_create_db(m_env, m_db, 1, 0, 0);
        if (st)
            return (st);
    }
    else {
        st=ham_create_ex(m_db, DB_PATH "test-ham.db", flags, 0664, &params[0]);
        if (st)
            return (st);
    }

    if (m_config->compression) {
        st=ham_enable_compression(m_db, 0, 0);
        if (st)
            return (st);
    }

    if (m_config->numeric) {
        st=ham_set_compare_func(m_db, my_compare_keys);
        if (st)
            return (st);
    }

    st=ham_cursor_create(m_db, 0, 0, &m_cursor);
    if (st)
        return (st);

    return (0);
}

ham_status_t 
hamsterdb::open()
{
    ham_status_t st;
    ham_u32_t flags=0;
    ham_parameter_t params[6];

    timer t(this, timer::misc);

    if (!m_env) {
        st=ham_env_new(&m_env);
        if (st)
            return (st);
    }

    if (!m_db) {
        st=ham_new(&m_db);
        if (st)
            return (st);
    }

    params[0].name=HAM_PARAM_DATA_ACCESS_MODE;
    params[0].value=m_config->data_access_mode;
    params[1].name=0;
    params[1].value=0;

    flags|=m_config->no_mmap?HAM_DISABLE_MMAP:0; 
    flags|=m_config->lock_excl?HAM_LOCK_EXCLUSIVE:0;
    flags|=m_config->cacheunlimited?HAM_CACHE_UNLIMITED:0;
    flags|=m_config->sort_dupes?HAM_SORT_DUPLICATES:0;
    flags|=m_config->enable_transactions?HAM_ENABLE_TRANSACTIONS:0;
    flags|=m_config->use_writethrough?HAM_WRITE_THROUGH:0;

    /*
     * aes encrypted databases are opened from an environment
     */
    if (m_config->aes_encrypt) {
        st=ham_env_open_ex(m_env, DB_PATH "test-ham.db", flags, 0);
        if (st)
            return (st);
        st=ham_env_enable_encryption(m_env, aeskey, 0);
        if (st)
            return (st);
        st=ham_env_open_db(m_env, m_db, 1, 0, &params[0]);
        if (st)
            return (st);
    }
    else {
        st=ham_open_ex(m_db, DB_PATH "test-ham.db", flags, &params[0]);
        if (st)
            return (st);
    }

    if (m_config->compression) {
        st=ham_enable_compression(m_db, 0, 0);
        if (st)
            return (st);
    }

    if (m_config->numeric) {
        st=ham_set_compare_func(m_db, my_compare_keys);
        if (st)
            return (st);
        if (m_config->sort_dupes) {
            st=ham_set_duplicate_compare_func(m_db, my_compare_keys);
            if (st)
                return (st);
        }
    }

    st=ham_cursor_create(m_db, 0, 0, &m_cursor);
    if (st)
        return (st);

    return (0);
}

ham_status_t 
hamsterdb::close()
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

    if (m_db) {
        st=ham_close(m_db, HAM_AUTO_CLEANUP);
        if (st)
            return (st);
        m_db=0;
    }

    if (m_env)
        st=ham_env_close(m_env, 0);
    return (st);
}

ham_status_t 
hamsterdb::flush()
{
    timer t(this, timer::misc);

    return (ham_flush(m_db, 0));
}

ham_status_t 
hamsterdb::insert(ham_key_t *key, ham_record_t *record)
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
hamsterdb::erase(ham_key_t *key)
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
hamsterdb::find(ham_key_t *key, ham_record_t *record)
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
hamsterdb::txn_begin(void)
{
    assert(m_txn==0);

    return (ham_txn_begin(&m_txn, m_db, 0));
}

ham_status_t 
hamsterdb::txn_commit(void)
{
    assert(m_txn!=0);

    ham_status_t st=ham_txn_commit(m_txn, 0);
    if (st)
        return (st);
    m_txn=0;
    return (0);
}

const char *
hamsterdb::get_name(void)
{
    return ("hamsterdb");
}


ham_status_t 
hamsterdb::check_integrity(void)
{
	return ham_check_integrity(m_db, 0);
}

void *
hamsterdb::create_cursor(void)
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
hamsterdb::get_previous(void *cursor, ham_key_t *key, 
                    ham_record_t *record, int flags)
{
    timer t(this, timer::cursor);

    if (m_config->direct_access && m_config->inmemory)
        flags|=HAM_DIRECT_ACCESS;

    return (ham_cursor_move((ham_cursor_t *)cursor, key, record, 
                    HAM_CURSOR_PREVIOUS|flags));
}

ham_status_t
hamsterdb::get_next(void *cursor, ham_key_t *key, ham_record_t *record, 
                int flags)
{
    timer t(this, timer::cursor);

    if (m_config->direct_access && m_config->inmemory)
        flags|=HAM_DIRECT_ACCESS;

    return (ham_cursor_move((ham_cursor_t *)cursor, key, record, 
                    HAM_CURSOR_NEXT|flags));
}

void 
hamsterdb::close_cursor(void *cursor)
{
    ham_status_t st=ham_cursor_close((ham_cursor_t *)cursor);
    if (st) {
        TRACE(("failed to close cursor: %d\n", st));
        exit(-1);
    }
}
