#include "porting.hpp"
#include "config.hpp"
#include "endian.hpp"
#include "engine.hpp"
#include "parser.hpp"
#include "hamsterdb.hpp"
#include "berkeleydb.hpp"
#include "misc.hpp"


engine::engine(config *c)
:   m_config(c), m_parser(0)
{
    m_db[0]=new hamsterdb(m_config);
    m_db[1]=new berkeleydb(m_config);
}

engine::~engine()
{
    for (int i=0; i<2; i++) {
        if (m_db[i]) {
            m_db[i]->close();
            delete m_db[i];
            m_db[i]=0;
        }
    }
}

void
engine::set_parser(parser *p)
{
    m_parser=p;
}

bool 
engine::create(bool numeric)
{
    if (numeric)
        m_config->numeric=true;

    for (int i=0; i<2; i++) {
        ham_status_t st=m_db[i]->create();
        if (st) {
            TRACE(("db[%d]: create failed w/ status %d\n", i, st));
            return (false);
        }
    }

    return (true);
}

bool 
engine::open(bool numeric)
{
    if (numeric)
        m_config->numeric=true;

    for (int i=0; i<2; i++) {
        ham_status_t st=m_db[i]->open();
        if (st) {
            TRACE(("db[%d]: open failed w/ status %d\n", i, st));
            return (false);
        }
    }

    return (true);
}

bool 
engine::insert(const char *keytok, const char *data)
{
    ham_u32_t numkey=0;
    ham_size_t data_size;
    ham_key_t key;
    ham_record_t rec;
    ham_status_t st[2];

    VERBOSE(("insert: key: %s, data: %s\n", keytok, data));

    memset(&key, 0, sizeof(key));
    memset(&rec, 0, sizeof(rec));

    /*
     * check flag NUMERIC_KEY
     */
    if (m_config->numeric) {
        numkey=strtoul(keytok, 0, 0);
        if (!numkey) {
            TRACE(("line %d: key is invalid\n", m_parser->get_lineno()));
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

    /*
     * allocate and initialize data 
     */
    data_size=strtoul(data, 0, 0);
    if (data_size) {
        if (data_size>m_config->data_size) {
            m_config->data_size=data_size;
            m_config->data_ptr=realloc(m_config->data_ptr, data_size);
            if (!m_config->data_ptr) {
                TRACE(("line %d: out of memory\n", m_parser->get_lineno()));
                return 0;
            }
        }
        /* always start with a random number - otherwise berkeleydb fails
         * too often when duplicate keys are inserted with duplicate
         * records */
        for (ham_size_t i=0; i<data_size; i++)
            ((char *)m_config->data_ptr)[i]=(m_parser->get_lineno()+i)&0xff;
        if (data_size>=sizeof(unsigned))
            *(unsigned *)m_config->data_ptr=m_parser->get_lineno();

        rec.data=m_config->data_ptr;
        rec.size=data_size;
    }

    for (int i=0; i<2; i++) {
        st[i]=m_db[i]->insert(&key, &rec);
        if (st[i])
            VERBOSE(("db[%d]: insert failed w/ status %d\n", i, st[i]));
    }

    return (compare_status(st));
}

bool 
engine::erase(const char *keytok)
{
    ham_u32_t numkey=0;
    ham_key_t key;
    ham_status_t st[2];

    VERBOSE(("erase: key: %s\n", keytok));

    memset(&key, 0, sizeof(key));

    /*
     * check flag NUMERIC_KEY
     */
    if (m_config->numeric) {
        numkey=strtoul(keytok, 0, 0);
        if (!numkey) {
            TRACE(("line %d: key is invalid\n", m_parser->get_lineno()));
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

    for (int i=0; i<2; i++) {
        st[i]=m_db[i]->erase(&key);
        if (st[i])
            VERBOSE(("db[%d]: erase failed w/ status %d\n", i, st[i]));
    }

    return (compare_status(st));
}

bool 
engine::find(const char *keytok)
{
    ham_u32_t numkey=0;
    ham_key_t key;
    ham_record_t rec[2];
    ham_status_t st[2];

    VERBOSE(("find: key: %s\n", keytok));

    memset(&key, 0, sizeof(key));
    memset(&rec[0], 0, sizeof(rec[0]));
    memset(&rec[1], 0, sizeof(rec[1]));

    /*
     * check flag NUMERIC_KEY
     */
    if (m_config->numeric) {
        numkey=strtoul(keytok, 0, 0);
        if (!numkey) {
            TRACE(("line %d: key is invalid\n", m_parser->get_lineno()));
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

    for (int i=0; i<2; i++) {
        st[i]=m_db[i]->find(&key, &rec[i]);
        if (st[i])
            VERBOSE(("db[%d]: find failed w/ status %d\n", i, st[i]));
    }

    if (!compare_records(&rec[0], &rec[1])) {
        TRACE(("record mismatch\n"));
        return false;
    }
    return (compare_status(st));
}

bool 
engine::fullcheck(void)
{
    ham_key_t key[2];
    ham_record_t rec[2];

    void *c[2];
    ham_status_t st[2];
    st[0]=m_db[0]->check_integrity();
    st[1]=m_db[1]->check_integrity();
    if (!compare_status(st))
        return false;

    c[0]=m_db[0]->create_cursor();
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

			rec[0].data = static_cast<char *>(m_config->puseralloc) + 65530;
			rec[0].size = USER_MALLOC_KEYRECSIZE - 65530;
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
            if (m_config->numeric)
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

bool 
engine::close(bool noreopen/* =false */)
{
    for (int i=0; i<2; i++) {
        ham_status_t st=m_db[i]->close();
        if (st) {
            TRACE(("db[%d]: close failed w/ status %d\n", i, st));
            return (false);
        }
    }

    if (!noreopen && m_config->reopen) {
        VERBOSE(("reopen\n"));
        if (!open(m_config->numeric))
            return (false);
        if (!fullcheck())
            return (false);
        if (!close(true))
            return (false);
    }

    for (int i=0; i<2; i++) {
        delete m_db[i];
        m_db[i]=0;
    }

    return (true);
}

bool 
engine::flush(void)
{
    for (int i=0; i<2; i++) {
        ham_status_t st=m_db[i]->flush();
        if (st) {
            TRACE(("db[%d]: flush failed w/ status %d\n", i, st));
            return (false);
        }
    }

    return (true);
}

bool 
engine::compare_records(ham_record_t *rec1, ham_record_t *rec2)
{
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

bool 
engine::compare_status(ham_status_t st[2])
{
    if (st[0]!=st[1]) {
        TRACE(("status mismatch - %d vs %d\n"
			"       ----> (%s) vs (%s)\n", 
			st[0], st[1],
			ham_strerror(st[0]), ham_strerror(st[1])));
        return false;
    }

    return (true);
}
