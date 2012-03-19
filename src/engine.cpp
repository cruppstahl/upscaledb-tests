#include <iostream>
#include <string>

#include "porting.hpp"
#include "config.hpp"
#include "endian.hpp"
#include "engine.hpp"
#include "parser.hpp"
#include "hamsterdb.hpp"
#include "berkeleydb.hpp"
#include "misc.hpp"


Engine::Engine(int id, config *c, Parser *p, const std::string &name)
:   m_config(c), m_parser(p), m_opcount(0)
{
    boost::mutex::scoped_lock lock(m_mutex);
    if (name=="hamsterdb")
        m_db=new Hamsterdb(id, m_config);
    else
        m_db=new Berkeleydb(id, m_config);
}

Engine::~Engine()
{
    boost::mutex::scoped_lock lock(m_mutex);
    if (m_db) {
        m_db->close_db();
        m_db->close_env();
        delete m_db;
        m_db=0;
    }
}

bool 
Engine::create_env()
{
    boost::mutex::scoped_lock lock(m_mutex);

    // the Environment is only created by thread #1
    if (!owns_env())
        return true;

    ham_status_t st;

    st=m_db->create_env();
    if (st) {
        TRACE(("db: create_env failed w/ status %d\n", st));
        return (false);
    }

    return (true);
}

bool 
Engine::create_db(bool numeric)
{
    boost::mutex::scoped_lock lock(m_mutex);
    ham_status_t st;

    if (numeric)
        m_config->set_numeric(true);

    st=m_db->create_db();
    if (st) {
        TRACE(("db: create_db failed w/ status %d\n", st));
        return (false);
    }

    if (m_config->txn_group)
        return (txn_begin());

    return (true);
}

bool 
Engine::open_env()
{
    boost::mutex::scoped_lock lock(m_mutex);
    ham_status_t st;

    // the Environment is only created by thread #1
    if (!owns_env())
        return true;

    st=m_db->open_env();
    if (st) {
        TRACE(("db: open_env failed w/ status %d\n", st));
        return (false);
    }

    return (true);
}

bool 
Engine::open_db(bool numeric)
{
    boost::mutex::scoped_lock lock(m_mutex);
    ham_status_t st;

    if (numeric)
        m_config->set_numeric(true);

    st=m_db->open_db();
    if (st) {
        TRACE(("db: open_db failed w/ status %d\n", st));
        return (false);
    }

    return (true);
}

bool 
Engine::insert(unsigned lineno, const char *keytok, const char *data)
{
    boost::mutex::scoped_lock lock(m_mutex);
    ham_u32_t numkey=0;
    ham_size_t data_size;
    ham_key_t key;
    ham_record_t rec;
    ham_status_t st;

    VERBOSE(("insert: key: %s, data: %s\n", keytok, data));

    memset(&key, 0, sizeof(key));
    memset(&rec, 0, sizeof(rec));

    /*
     * check flag NUMERIC_KEY
     */
    if (m_config->is_numeric()) {
        numkey=strtoul(keytok, 0, 0);
        if (!numkey) {
            TRACE(("key is invalid\n"));
            return (false);
        }
        numkey=ham_h2db32(numkey);
        key.data=(void *)&numkey;
        key.size=sizeof(numkey);
    }
    else {
        key.data=(void *)keytok;
        key.size=(ham_size_t)strlen(keytok)+1;
    }

    /*
     * allocate and initialize data 
     */
    data_size=strtoul(data, 0, 0);

    /*
     * berkeleydb: duplicates are not allowed with identical records,
     * and hamsterdb also forbids this if duplicates are sorted, therefore
     * the records are "unique-ified" by patching the line number in the
     * beginning
     */
    if (m_config->sort_dupes && data_size==0)
        data_size=sizeof(unsigned);
    if (data_size) {
        if (data_size>m_config->data_size) {
            m_config->data_size=data_size;
            m_config->data_ptr=realloc(m_config->data_ptr, data_size);
        }
        /* always start with a random number - otherwise berkeleydb fails
         * too often when duplicate keys are inserted with duplicate
         * records */
        for (ham_size_t i=0; i<data_size; i++)
            ((char *)m_config->data_ptr)[i]=(lineno+i)&0xff;
        if (data_size>=sizeof(unsigned))
            *(unsigned *)m_config->data_ptr=lineno;

        rec.data=m_config->data_ptr;
        rec.size=data_size;
    }

    st=m_db->insert(&key, &rec);
    if (st)
        VERBOSE(("db: insert failed w/ status %d\n", st));

    if (m_config->txn_group>0)
        return (inc_opcount());
    return (true);
}

bool 
Engine::erase(const char *keytok)
{
    boost::mutex::scoped_lock lock(m_mutex);
    ham_u32_t numkey=0;
    ham_key_t key;
    ham_status_t st;

    VERBOSE(("erase: key: %s\n", keytok));

    memset(&key, 0, sizeof(key));

    /*
     * check flag NUMERIC_KEY
     */
    if (m_config->is_numeric()) {
        numkey=strtoul(keytok, 0, 0);
        if (!numkey) {
            TRACE(("key is invalid\n"));
            return (false);
        }
        numkey=ham_h2db32(numkey);
        key.data=(void *)&numkey;
        key.size=sizeof(numkey);
    }
    else {
        key.data=(void *)keytok;
        key.size=(ham_size_t)strlen(keytok);
    }

    st=m_db->erase(&key);
    if (st)
        VERBOSE(("db: erase failed w/ status %d\n", st));

    if (m_config->txn_group>0)
        return (inc_opcount());

    return (true);
}

bool 
Engine::find(const char *keytok)
{
    boost::mutex::scoped_lock lock(m_mutex);
    ham_u32_t numkey=0;
    ham_key_t key;
    ham_record_t rec;
    ham_status_t st;

    VERBOSE(("find: key: %s\n", keytok));

    memset(&key, 0, sizeof(key));
    memset(&rec, 0, sizeof(rec));

    /*
     * check flag NUMERIC_KEY
     */
    if (m_config->is_numeric()) {
        numkey=strtoul(keytok, 0, 0);
        if (!numkey) {
            TRACE(("key is invalid\n"));
            return (false);
        }
        numkey=ham_h2db32(numkey);
        key.data=(void *)&numkey;
        key.size=sizeof(numkey);
    }
    else {
        key.data=(void *)keytok;
        key.size=(ham_size_t)strlen(keytok);
    }

    st=m_db->find(&key, &rec);
    if (st)
        VERBOSE(("db: find failed w/ status %d\n", st));

    if (m_config->txn_group>0)
        return (inc_opcount());

    return (true);
}

#if 0
bool 
Engine::fullcheck(void)
{
    boost::mutex::scoped_lock lock(m_mutex);
    ham_key_t key[2];
    ham_record_t rec[2];

    void *c[2]={0};
    ham_status_t st[2]={0};
    if (m_db[0])
        st[0]=m_db[0]->check_integrity();
    if (m_db[1])
        st[1]=m_db[1]->check_integrity();
    if (!compare_status(st))
        return false;

    // fullcheck does not make sense if berkeleydb is disabled
    if (m_config->no_bdb)
        return true;

    if (m_db[0])
        c[0]=m_db[0]->create_cursor();
    if (m_db[1])
        c[1]=m_db[1]->create_cursor();

    for(;;) {
        memset(key, 0, sizeof(key));
        memset(rec, 0, sizeof(rec));

        if (m_config->puseralloc) {
            key[0].data=m_config->puseralloc;
            key[0].flags=HAM_KEY_USER_ALLOC;
            /* keysize is a 16-bit value! */
            assert(USER_MALLOC_KEYRECSIZE > 65530);
            key[0].size=65530;

            rec[0].data=static_cast<char *>(m_config->puseralloc)+65530;
            rec[0].size=USER_MALLOC_KEYRECSIZE-65530;
            rec[0].flags=HAM_RECORD_USER_ALLOC;
        }

        /* first: berkeley db */
        if (m_config->fullcheck_find) {
            st[1]=m_db[1]->get_next(c[1], &key[1], &rec[1], 
                            HAM_SKIP_DUPLICATES);
        }
        else if (m_config->fullcheck_backwards) {
            st[1]=m_db[1]->get_previous(c[1], &key[1], &rec[1], 0);
        }
        else {
            st[1]=m_db[1]->get_next(c[1], &key[1], &rec[1], 0);
        }

        /* then: hamster db */
        if (m_config->fullcheck_find) {
            /*
             * different behaviour for BDB and hamsterdb:
             * 
             * since BDB is used in this mode to fetch keys from its database 
             * and hamsterdb can only /look/ for keys, it's no use feeding 
             * hamsterdb an (illegal) key to look for when BDB didn't deliver 
             * one.
             *
             * This mode does NOT discover keys lurking in the hamsterdb 
             * database which are NOT in the BDB database. Use regular 
             * fullcheck or fullcheck-backwards to cover that sort of thing.
             */
            if (st[1]==HAM_KEY_NOT_FOUND)
                st[0]=st[1];
            else
                st[0]=m_db[0]->find(&key[1], &rec[0]); /* !! */
        }
        else if (m_config->fullcheck_backwards)
            st[0]=m_db[0]->get_previous(c[0], &key[0], &rec[0], 0);
        else
            st[0]=m_db[0]->get_next(c[0], &key[0], &rec[0], 0);

        if (m_config->verbose>1) {
            if (m_config->is_numeric())
                printf("fullcheck: %d/%d, keys %d/%d, blob size %d/%d\n", 
                    st[0], st[1], 
                    key[0].data ? *(int *)key[0].data : 0, 
                    key[1].data ? *(int *)key[1].data : 0,
                    rec[0].size, rec[1].size);
            else
                printf("fullcheck: %d/%d, keys %s/%s, blob size %d/%d\n", 
                    st[0], st[1], 
                    key[0].data ? (char *)key[0].data : "(null)", 
                    key[1].data ? (char *)key[1].data : "(null)",
                    rec[0].size, rec[1].size);
        }

/*
if (key[0].data && *(unsigned *)key[0].data==997) {
	printf("hit\n");
}
*/

        if (!compare_status(st))
            return false;
        if (st[0]!=0)
            break;

        if (!compare_records(&rec[0], &rec[1])) {
            TRACE(("record mismatch\n"));
            return false;
        }
    }

    m_db[0]->close_cursor(c[0]);
    m_db[1]->close_cursor(c[1]);
    return (true);
}
#endif

bool 
Engine::close_db()
{
    boost::mutex::scoped_lock lock(m_mutex);
    ham_status_t st=m_db->close_db();
    if (st) {
        TRACE(("db: close_db failed w/ status %d\n", st));
        return (false);
    }

    return (true);
}

bool 
Engine::close_env()
{
    boost::mutex::scoped_lock lock(m_mutex);

    // collect metrics before the memory allocator is deleted in ham_env_close
    m_db->collect_metrics();

    // the Environment is only closed by thread #1
    if (!owns_env())
        return true;

    ham_status_t st=m_db->close_env();
    if (st) {
        TRACE(("db: close failed w/ status %d\n", st));
        return (false);
    }

#if 0
    if (!noreopen && m_config->reopen) {
        VERBOSE(("reopen\n"));
        if (!open(m_config->is_numeric()))
            return (false);
        //if (!fullcheck()) TODO wieder rein
            //return (false);
        if (!close_db())
            return (false);
        if (!close_env())
            return (false);
    }
#endif

    return (true);
}

bool 
Engine::flush(void)
{
    boost::mutex::scoped_lock lock(m_mutex);
    ham_status_t st=m_db->flush();
    if (st) {
        TRACE(("db: flush failed w/ status %d\n", st));
        return (false);
    }

    return (true);
}

bool 
Engine::txn_begin(void)
{
    boost::mutex::scoped_lock lock(m_mutex);
    ham_status_t st=m_db->txn_begin();
    if (st) {
        TRACE(("db: txn_begin failed w/ status %d\n", st));
        return (false);
    }

    return (true);
}

bool 
Engine::txn_commit(void)
{
    boost::mutex::scoped_lock lock(m_mutex);
    ham_status_t st=m_db->txn_commit();
    if (st) {
        TRACE(("db: txn_begin failed w/ status %d\n", st));
        return (false);
    }

    return (true);
}

#if 0
bool 
Engine::compare_records(ham_record_t *rec1, ham_record_t *rec2)
{
    boost::mutex::scoped_lock lock(m_mutex);
    if (rec1->size!=rec2->size)
        return (false);
    if (!rec1->size && !rec2->size)
        return (true);
    if ((rec1->data && !rec2->data) || (rec2->data && !rec1->data))
        return (false);
    if (!rec1->data && !rec2->data)
        return (true);
    return (memcmp(rec1->data, rec2->data, rec1->size)==0);
}
#endif

bool 
Engine::inc_opcount(void)
{
    boost::mutex::scoped_lock lock(m_mutex);
    if (++m_opcount>=m_config->txn_group) {
        if (!txn_commit())
            return (false);
        if (!txn_begin())
            return (false);
        m_opcount=0;
    }

    return (true);
}

#if 0
bool 
Engine::compare_status(ham_status_t st[2])
{
    boost::mutex::scoped_lock lock(m_mutex);
    if (st[0]!=st[1]) {
        TRACE(("status mismatch - hamsterdb %d vs berkeleydb %d\n"
            "       ----> (%s) vs (%s)\n", 
            st[0], st[1],
            ham_strerror(st[0]), ham_strerror(st[1])));
        return false;
    }

    return (true);
}
#endif

