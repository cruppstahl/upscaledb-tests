
#include "porting.hpp"
#include "porting.hpp"
#include "config.hpp"
#include "engine.hpp"
#include "parser.hpp"
#include "getopts.h"
#include "hamsterdb.hpp"
#include "berkeleydb.hpp"
#include "metrics.hpp"
#include <iostream>
#include <boost/thread.hpp>


#define ARG_HELP                        1
#define ARG_VERBOSE                     2
#define ARG_QUIET                       3
#define ARG_INMEMORY                   10
#define ARG_OVERWRITE                  11
#define ARG_PROGRESS                   12
#define ARG_DISABLE_MMAP               13
#define ARG_PAGESIZE                   14
#define ARG_KEYSIZE                    15
#define ARG_CACHESIZE                  16
#define ARG_REOPEN                     18
#define ARG_USERALLOC                  19
#define ARG_USE_CURSORS                23
#define ARG_NUMERIC                    24
#define ARG_LOCK_EXCLUSIVE             25
#define ARG_DUPLICATE                  26
#define ARG_FULLCHECK_FIND             27
#define ARG_FULLCHECK_BACKWARDS        28
#define ARG_DUPLICATE_FIRST            31
#define ARG_AES_ENCRYPTION             32
#define ARG_COMPRESSION                33
#define ARG_RECOVERY                   34
#define ARG_CACHEUNLIMITED             35
#define ARG_DATA_ACCESS_MODE           36
#define ARG_HINTING                    37
#define ARG_SORT_DUPLICATES            38
#define ARG_DIRECT_ACCESS              39
#define ARG_USE_TRANSACTIONS           41
#define ARG_WRITETHROUGH               42
#define ARG_NO_BDB                     43
#define ARG_NUM_THREADS                44

Metrics *Metrics::instance;

/*
 * command line parameters
 */
static option_t opts[]={
    { 
        ARG_HELP,               // symbolic name of this option
        "h",                    // short option 
        "help",                 // long option 
        "this help screen",     // help string
        0 },                    // no flags
    {
        ARG_VERBOSE,
        "v",
        "verbose",
        "be verbose",
        0 },
    {
        ARG_QUIET,
        "q",
        "quiet",
        "do not print profiling metrics",
        0 },
    {
        ARG_INMEMORY,
        0,
        "inmemorydb",
        "create in-memory-databases (if available)",
        0 },
    {
        ARG_OVERWRITE,
        0,
        "overwrite",
        "overwrite existing keys",
        0 },
    {
        ARG_DUPLICATE,
        0,
        "duplicate",
        "enable duplicate keys",
        0 },
    {
        ARG_DUPLICATE_FIRST,
        0,
        "duplicate-first",
        "insert duplicate keys as the FIRST duplicate",
        0 },
    {
        ARG_USE_CURSORS,
        0,
        "use-cursors",
        "use cursors for insert/erase",
        0 },
    {
        ARG_AES_ENCRYPTION,
        0,
        "use-aes",
        "use aes encryption",
        0 },
    {
        ARG_COMPRESSION,
        0,
        "use-compression",
        "use zlib compression",
        0 },
    {
        ARG_RECOVERY,
        0,
        "use-recovery",
        "use recovery",
        0 },
    {
        ARG_NUMERIC,
        0,
        "numeric",
        "force use of numeric (integer) keys",
        0 },
    {
        ARG_LOCK_EXCLUSIVE,
        0,
        "lock-ex",
        "lock the file exclusively",
        0 },
    {
        ARG_PROGRESS,
        0,
        "progress",
        "show progress",
        0 },
    {
        ARG_DISABLE_MMAP,
        0,
        "disable-mmap",
        "disable mmap",
        0 },
    {
        ARG_REOPEN,
        0,
        "reopen",
        "call OPEN/FULLCHECK/CLOSE after each close",
        0 },
    {
        ARG_FULLCHECK_FIND,
        0,
        "fullcheck-find",
        "use ham_find in fullcheck",
        0 },
    {
        ARG_FULLCHECK_BACKWARDS,
        0,
        "fullcheck-backwards",
        "go backwards in fullcheck",
        0 },
    {
        ARG_USERALLOC,
        0,
        "useralloc",
        "ham_find: use flag HAM_RECORD_USER_ALLOC",
        0 },
    {
        ARG_PAGESIZE,
        0,
        "pagesize",
        "set the pagesize (use 0 for default)",
        0 },
    {
        ARG_KEYSIZE,
        0,
        "keysize",
        "set the keysize (use 0 for default)",
        0 },
    {
        ARG_CACHESIZE,
        0,
        "cachesize",
        "set the cachesize (use 0 for default)",
        0 },
    {
        ARG_CACHEUNLIMITED,
        0,
        "cacheunlimited",
        "use an unlimited cache",
        0 },
    {
        ARG_DATA_ACCESS_MODE,
        0,
        "data-access-mode",
        "sets data access mode (DAM) - comma separated:\n"
        "\tHAM_DAM_SEQUENTIAL_INSERT, HAM_DAM_RANDOM_WRITE",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_HINTING,
        0,
        "hints",
        "sets hinting flags - comma separated:\n"
        "\tHAM_HINT_RANDOM_ACCESS, HAM_HINT_SEQUENTIAL,\n"
        "\tHAM_HINT_UBER_FAST_ACCESS, HAM_HINT_APPEND, HAM_HINT_PREPEND",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_SORT_DUPLICATES,
        0,
        "sort-duplicates",
        "enables sorting for duplicate keys",
        0 },
    {
        ARG_DIRECT_ACCESS,
        0,
        "direct-access",
        "sets HAM_DIRECT_ACCESS flag",
        0 },
    {
        ARG_USE_TRANSACTIONS,
        0,
        "use-transactions",
        "use Transactions; arguments are \n"
        "\t'tmp' - create temp. Transactions;\n"
        "\tN - (number) group N statements into a Transaction;\n"
        "\t'all' - group the whole test into a single Transaction",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_WRITETHROUGH,
        0,
        "writethrough",
        "sets the HAM_WRITE_THROUGH flag",
        0 },
    {
        ARG_NO_BDB,
        "no_bdb",
        "no-berkeleydb",
        "disables berkeleydb (and FULLCHECK)",
        0 },
    {
        ARG_NUM_THREADS,
        0,
        "num-threads",
        "sets the number of threads (default: 1)",
        GETOPTS_NEED_ARGUMENT },
    { 0, 0, 0, 0, 0 }
};

static void
parse_config(int argc, char **argv, config *c)
{
    unsigned opt;
    char *param;
	
    getopts_init(argc, argv, "test");

    /*
     * parse command line parameters
     */
    while ((opt=getopts(&opts[0], &param))) {
        if (opt==ARG_HELP) {
            getopts_usage(&opts[0]);
            exit(0);
        }
        else if (opt==ARG_QUIET) {
            c->quiet=true;
        }
        else if (opt==ARG_VERBOSE) {
            c->verbose++;
        }
        //else if (opt==ARG_FILE) {
            //c->filename=param;
        //}
        else if (opt==ARG_INMEMORY) {
            c->inmemory=true;
        }
        else if (opt==ARG_OVERWRITE) {
            if (c->duplicate) {
                printf("invalid combination: overwrite && duplicate\n");
                exit(-1);
            }
            c->overwrite=true;
        }
        else if (opt==ARG_DUPLICATE) {
            if (c->overwrite) {
                printf("invalid combination: overwrite && duplicate\n");
                exit(-1);
            }
            c->duplicate=true;
        }
        else if (opt==ARG_DUPLICATE_FIRST) {
            if (!c->use_cursors) {
                printf("'duplicate-first' needs 'use-cursors'\n");
                exit(-1);
            }
            c->dupe_flags=HAM_DUPLICATE_INSERT_FIRST;
            c->duplicate=true;
        }
        else if (opt==ARG_USE_CURSORS) {
            c->use_cursors=true;
        }
        else if (opt==ARG_AES_ENCRYPTION) {
#if 0
			c->aes_encrypt=true;
#endif
        }
        else if (opt==ARG_COMPRESSION) {
            c->compression=true;
        }
        else if (opt==ARG_RECOVERY) {
            c->recovery=true;
        }
        else if (opt==ARG_NUMERIC) {
            c->numeric=true;
        }
        else if (opt==ARG_PROGRESS) {
            c->progress=true;
        }
        else if (opt==ARG_DISABLE_MMAP) {
            c->no_mmap=true;
        }
        else if (opt==ARG_PAGESIZE) {
            c->pagesize=strtoul(param, 0, 0);
        }
        else if (opt==ARG_KEYSIZE) {
            c->keysize=strtoul(param, 0, 0);
        }
        else if (opt==ARG_CACHESIZE) {
            c->cachesize=strtoul(param, 0, 0);
        }
        else if (opt==ARG_CACHEUNLIMITED) {
            c->cacheunlimited=true;
        }
        else if (opt==ARG_DATA_ACCESS_MODE) {
            if (strstr(param, "HAM_DAM_SEQUENTIAL_INSERT"))
                c->data_access_mode|=HAM_DAM_SEQUENTIAL_INSERT;
            if (strstr(param, "HAM_DAM_RANDOM_WRITE"))
                c->data_access_mode|=HAM_DAM_RANDOM_WRITE;
            if (strstr(param, "HAM_DAM_ENFORCE_PRE110_FORMAT"))
                c->data_access_mode|=HAM_DAM_ENFORCE_PRE110_FORMAT;
            if (param && !c->data_access_mode) {
                printf("invalid or missing parameter for 'data access mode'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_HINTING) {
            if (strstr(param, "HAM_HINT_RANDOM_ACCESS"))
                c->hints|=HAM_HINT_RANDOM_ACCESS;
            if (strstr(param, "HAM_HINT_SEQUENTIAL"))
                c->hints|=HAM_HINT_SEQUENTIAL;
            if (strstr(param, "HAM_HINT_UBER_FAST_ACCESS"))
                c->hints|=HAM_HINT_UBER_FAST_ACCESS;
            if (strstr(param, "HAM_HINT_APPEND"))
                c->hints|=HAM_HINT_APPEND;
            if (strstr(param, "HAM_HINT_PREPEND"))
                c->hints|=HAM_HINT_PREPEND;
            if (param && !c->hints) {
                printf("invalid or missing parameter for 'hints'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_SORT_DUPLICATES) {
            c->sort_dupes=true;
        }
        else if (opt==ARG_DIRECT_ACCESS) {
            c->direct_access=true;
        }
        else if (opt==ARG_WRITETHROUGH) {
            c->use_writethrough=true;
        }
        else if (opt==ARG_NO_BDB) {
            c->no_bdb=true;
        }
        else if (opt==ARG_USE_TRANSACTIONS) {
            c->enable_transactions=true;
            if (strcmp("tmp", param)==0)
                c->txn_group=0;
            else if (strcmp("all", param)==0)
                c->txn_group=0xffffffff;
            else {
                c->txn_group=strtoul(param, 0, 0);
                if (!c->txn_group) {
                    printf("invalid parameter for 'use-transactions'\n");
                    exit(-1);
                }
            }
        }
        else if (opt==ARG_REOPEN) {
            c->reopen=true;
        }
        else if (opt==ARG_LOCK_EXCLUSIVE) {
            c->lock_excl=true;
        }
        else if (opt==ARG_USERALLOC) {
            c->puseralloc=malloc(USER_MALLOC_KEYRECSIZE);
            if (!c->puseralloc) {
                printf("out of memory\n");
                exit(-1);
            }
        }
        else if (opt==ARG_FULLCHECK_FIND) {
            c->fullcheck_find=true;
        }
        else if (opt==ARG_FULLCHECK_BACKWARDS) {
            c->fullcheck_backwards=true;
        }
        else if (opt==ARG_NUM_THREADS) {
            c->num_threads=strtoul(param, 0, 0);
            if (!c->num_threads) {
                printf("invalid parameter for 'num-threads'\n");
                exit(-1);
            }
        }
        else if (opt==GETOPTS_PARAMETER) {
            c->filename=param;
        }
        else {
            printf("unknown parameter '%s'\n", param);
            exit(-1);
        }
    }
}

class Thread
{
  public:
    Thread(int id, config &c, Parser &p)
      : m_id(id), m_fail(false) {
        run();
    }

    void run() {
        printf("running thread %d\n", m_id);
    }

    bool success() {
        return !m_fail;
    }

    void join() {
        m_thread.join();
    }

  private:
    int m_id;
    bool m_fail;
    boost::thread m_thread;
};

int
main(int argc, char **argv)
{
    config c;
    parse_config(argc, argv, &c);

    Engine e(&c);
    Parser p(&c, &e, c.filename, 0);
    e.set_parser(&p);

    bool ok=true;
    if (c.num_threads==1) {
        ok=p.process();
    }
    else {
        std::vector<Thread *> threads;
        for (int i=0; i<c.num_threads; i++) {
            threads.push_back(new Thread(i+1, c, p));
        }
        for (int i=0; i<c.num_threads; i++) {
            threads[i]->join();
            if (!threads[i]->success())
                ok=false;
            delete threads[i];
        }
    }

    if (ok)
        printf("[OK] %s\n", c.filename);
    else
        printf("[FAIL] %s\n", c.filename);

    if (ok && !c.quiet) {
        if (e.get_db(0))
            e.get_db(0)->collect_metrics();
        if (e.get_db(1))
            e.get_db(1)->collect_metrics();
        Metrics::get_instance()->print();
    }

	return (ok ? 0 : 1);
}
