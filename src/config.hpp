
#ifndef CONFIG_HPP__
#define CONFIG_HPP__


#define USER_MALLOC_KEYRECSIZE  (1024*1024*64)


#define DB_PATH ""


typedef struct config
{
    config()
    {
        profile=false;
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
        puseralloc=0;
        filename=0;
        data_size=0;
        data_ptr=0;
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
        output_xml=false;
        enable_transactions=false;
        txn_group=0;
        use_writethrough=false;
    }

    ~config()
    {
        if (puseralloc) {
            free(puseralloc);
            puseralloc=0;
        }
        if (data_ptr) {
            free(data_ptr);
            data_ptr=0;
        }
    }

    bool profile;
    unsigned verbose;
    bool overwrite;
    bool duplicate;
    bool inmemory;
    bool use_cursors;
    bool numeric;
    bool progress;
    bool no_mmap;
    unsigned keysize;
    unsigned pagesize;
    unsigned cachesize;
    bool reopen;
    bool lock_excl;
    void *puseralloc;
    const char *filename;
    unsigned data_size;
    void *data_ptr;
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
    bool output_xml;
    bool enable_transactions;
    unsigned txn_group;
    bool use_writethrough;
} config;

#endif /* CONFIG_HPP__ */
