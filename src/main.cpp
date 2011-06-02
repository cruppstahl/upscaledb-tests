
#include "porting.hpp"
#include "porting.hpp"
#include "config.hpp"
#include "engine.hpp"
#include "parser.hpp"
#include "getopts.h"



#define ARG_HELP                        1
#define ARG_VERBOSE                     2
#define ARG_PROFILE                     3
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
#define ARG_OUTPUT_XML                 40
#define ARG_USE_TRANSACTIONS           41

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
        ARG_PROFILE,
        "p",
        "profile",
        "enable profiling",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_INMEMORY,
        "inmem",
        "inmemorydb",
        "create in-memory-databases (if available)",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_OVERWRITE,
        "over",
        "overwrite",
        "overwrite existing keys",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_DUPLICATE,
        "dupe",
        "duplicate",
        "enable duplicate keys",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_DUPLICATE_FIRST,
        "dupe-f",
        "duplicate-first",
        "insert duplicate keys as the FIRST duplicate",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_USE_CURSORS,
        "cursors",
        "use-cursors",
        "use cursors for insert/erase",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_AES_ENCRYPTION,
        "aes",
        "use-aes",
        "use aes encryption",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_COMPRESSION,
        "compress",
        "use-compression",
        "use zlib compression",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_RECOVERY,
        "recovery",
        "use-recovery",
        "use recovery",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_NUMERIC,
        "num",
        "numeric",
        "force use of numeric (integer) keys",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_LOCK_EXCLUSIVE,
        "lex",
        "lock_ex",
        "lock the file exclusively",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_PROGRESS,
        "prog",
        "progress",
        "show progress",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_DISABLE_MMAP,
        "no-mmap",
        "disable-mmap",
        "disable mmap",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_REOPEN,
        "reopen",
        "reopen",
        "call OPEN/FULLCHECK/CLOSE after each close",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_FULLCHECK_FIND,
        "ff",
        "fullcheck-find",
        "use ham_find in fullcheck",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_FULLCHECK_BACKWARDS,
        "fb",
        "fullcheck-backwards",
        "go backwards in fullcheck",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_USERALLOC,
        "usr",
        "useralloc",
        "ham_find: use flag HAM_RECORD_USER_ALLOC",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_PAGESIZE,
        "ps",
        "pagesize",
        "set the pagesize (use 0 for default)",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_KEYSIZE,
        "ks",
        "keysize",
        "set the keysize (use 0 for default)",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_CACHESIZE,
        "cs",
        "cachesize",
        "set the cachesize (use 0 for default)",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_CACHEUNLIMITED,
        "cu",
        "cacheunlimited",
        "use an unlimited cache",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_DATA_ACCESS_MODE,
        "dam",
        "data_access_mode",
        "sets data access mode (DAM) - comma separated: "
            "HAM_DAM_SEQUENTIAL_INSERT, HAM_DAM_RANDOM_WRITE",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_HINTING,
        "hints",
        "hints",
        "sets hinting flags - comma separated: "
            "HAM_HINT_RANDOM_ACCESS, HAM_HINT_SEQUENTIAL, "
            "HAM_HINT_UBER_FAST_ACCESS, HAM_HINT_APPEND, HAM_HINT_PREPEND",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_SORT_DUPLICATES,
        "sd",
        "sort-duplicates",
        "enables sorting for duplicate keys",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_DIRECT_ACCESS,
        "dac",
        "direct_access",
        "sets HAM_DIRECT_ACCESS flag",
        GETOPTS_NEED_ARGUMENT },
    {
        ARG_OUTPUT_XML,
        "xml",
        "output_xml",
        "create XML output",
        0 },
    {
        ARG_USE_TRANSACTIONS,
        "txn",
        "use_transactions",
        "use Transactions; argument is 'tmp' - create temp. Transactions; \n"
        "\tN - (number) group N statements into a Transaction;\n"
        "\t'all' - group the whole test into a single Transaction",
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
        else if (opt==ARG_PROFILE) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->profile=true;
            else {
                printf("invalid or missing parameter for 'profile'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_VERBOSE) {
            c->verbose++;
        }
        //else if (opt==ARG_FILE) {
            //c->filename=param;
        //}
        else if (opt==ARG_INMEMORY) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->inmemory=true;
            else {
                printf("invalid or missing parameter for 'inmemory'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_OVERWRITE) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y') {
                if (c->duplicate) {
                    printf("invalid combination: overwrite && duplicate\n");
                    exit(-1);
                }
                c->overwrite=true;
            }
            else {
                printf("invalid or missing parameter for 'overwrite'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_DUPLICATE) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y') {
                if (c->overwrite) {
                    printf("invalid combination: overwrite && duplicate\n");
                    exit(-1);
                }
                c->duplicate=true;
            }
            else {
                printf("invalid or missing parameter for 'duplicate'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_DUPLICATE_FIRST) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y') {
                if (!c->use_cursors) {
                    printf("'duplicate-first' needs 'use-cursors'\n");
                    exit(-1);
                }
                c->dupe_flags=HAM_DUPLICATE_INSERT_FIRST;
                c->duplicate=true;
            }
            else {
                printf("invalid or missing parameter for 'duplicate-first'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_USE_CURSORS) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->use_cursors=true;
            else {
                printf("invalid or missing parameter for 'use_cursors'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_AES_ENCRYPTION) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
			{
#if 0
				c->aes_encrypt=true;
#endif
			}
			else {
                printf("invalid or missing parameter for aes encryption\n");
                exit(-1);
            }
        }
        else if (opt==ARG_COMPRESSION) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->compression=true;
            else {
                printf("invalid or missing parameter for compression\n");
                exit(-1);
            }
        }
        else if (opt==ARG_RECOVERY) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->recovery=true;
            else {
                printf("invalid or missing parameter for compression\n");
                exit(-1);
            }
        }
        else if (opt==ARG_NUMERIC) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->numeric=true;
            else {
                printf("invalid or missing parameter for 'numeric'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_PROGRESS) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->progress=true;
            else {
                printf("invalid or missing parameter for 'progress'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_DISABLE_MMAP) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->no_mmap=true;
            else {
                printf("invalid or missing parameter for 'no_mmap'\n");
                exit(-1);
            }
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
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->cacheunlimited=true;
            else {
                printf("invalid or missing parameter for 'cacheunlimited'\n");
                exit(-1);
            }
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
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->sort_dupes=true;
            else {
                printf("invalid or missing parameter for 'sort-duplicates'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_DIRECT_ACCESS) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->direct_access=true;
            else {
                printf("invalid or missing parameter for 'direct_access'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_OUTPUT_XML) {
            c->output_xml=true;
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
                    printf("invalid parameter for 'use_transactions'\n");
                    exit(-1);
                }
            }
        }
        else if (opt==ARG_REOPEN) {
            c->reopen=true;
        }
        else if (opt==ARG_LOCK_EXCLUSIVE) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->lock_excl=true;
            else {
                printf("invalid or missing parameter for 'lock_excl'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_USERALLOC) {
            c->puseralloc=malloc(USER_MALLOC_KEYRECSIZE);
            if (!c->puseralloc) {
                printf("out of memory\n");
                exit(-1);
            }
        }
        else if (opt==ARG_FULLCHECK_FIND) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->fullcheck_find=true;
            else {
                printf("invalid or missing parameter for 'fullcheck-find'\n");
                exit(-1);
            }
        }
        else if (opt==ARG_FULLCHECK_BACKWARDS) {
            if (param[0]=='1' || param[0]=='y' || param[0]=='Y')
                c->fullcheck_backwards=true;
            else {
                printf("invalid or missing parameter for 'fullcheck-backwards'\n");
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

int
main(int argc, char **argv)
{
    config c;
    parse_config(argc, argv, &c);

    engine e(&c);
    parser p(&c, &e, c.filename, 0);
    e.set_parser(&p);

    if (c.output_xml) {
        printf("<test-results>\n");
        printf("\t<machine>\n");
        printf("\t\t<name></name>\n");
        printf("\t\t<architecture></architecture>\n");
        printf("\t</machine>\n");
        printf("\t<test>\n");
        printf("\t\t<path>%s</path>\n", c.filename);
        printf("\t\t<argument-list>\n");
        printf("\t\t\t<argument>\n");
        printf("\t\t\t\t<name></name>\n");
        printf("\t\t\t\t<value></value>\n");
        printf("\t\t\t</argument>\n");
        printf("\t\t</argument-list>\n");
    }

    bool ok = p.process();

    if (c.output_xml) {
        printf("\t\t<result>%d</result>\n", ok ? 0 : 1);
        printf("\t</test>\n");
        printf("</test-results>\n");
    }

	return (ok ? 0 : 1);
}
