
#ifndef CONFIG_HPP__
#define CONFIG_HPP__

#include <malloc.h>
#include <boost/thread.hpp>

#define USER_MALLOC_KEYRECSIZE  (1024*1024*64)


#define DB_PATH ""


typedef struct config
{
    config()
    {
        profile=true;
        verbose=0;
        overwrite=false;
        duplicate=false;
        inmemory=false;
        use_cursors=false;
        numeric=false;
        progress=false;
        no_mmap=false;
        keysize=0;
        pagesize=0;
        cachesize=0;
        reopen=false;
        lock_excl=false;
        filename=0;
        fullcheck_find=false;
        fullcheck_backwards=false;
        dupe_flags=0;
        aes_encrypt=false;
        compression=false;
        recovery=false;
        cacheunlimited=false;
        data_access_mode=0;
        hints=0;
        sort_dupes=false;
        direct_access=false;
        enable_transactions=false;
        txn_group=0;
        use_writethrough=false;
        quiet=false;
        no_bdb=false;
        num_threads=1;
    }

    void set_numeric(bool b) {
        boost::mutex::scoped_lock lock(m_mutex);
        numeric=b;
    }
    bool is_numeric() {
        boost::mutex::scoped_lock lock(m_mutex);
        return numeric;
    }

    bool profile;
    unsigned verbose;
    bool overwrite;
    bool duplicate;
    bool inmemory;
    bool use_cursors;
    bool progress;
    bool no_mmap;
    unsigned keysize;
    unsigned pagesize;
    unsigned cachesize;
    bool reopen;
    bool lock_excl;
    const char *filename;
    bool fullcheck_find;
    bool fullcheck_backwards;
    unsigned dupe_flags;
    bool aes_encrypt;
    bool compression;
    bool recovery;
    bool cacheunlimited;
    unsigned data_access_mode;
    unsigned hints;
    bool sort_dupes;
    bool direct_access;
    bool enable_transactions;
    unsigned txn_group;
    bool use_writethrough;
    bool quiet;
    bool no_bdb;
    unsigned num_threads;
    boost::mutex m_mutex;

  private:
    bool numeric;
} config;

#endif /* CONFIG_HPP__ */
