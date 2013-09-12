/**
 * Copyright (C) 2005-2013 Christoph Rupp (chris@crupp.de).
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * See files COPYING.* for License information.
 */


#include "porting.h"
#include "configuration.h"
#include "engine.h"
#include "parser.h"
#include "getopts.h"
#include "hamsterdb.h"
#include "berkeleydb.h"
#include "metrics.h"
#include "controller.h"
#include "thread.h"
#include <iostream>


#define ARG_HELP                    1
#define ARG_VERBOSE                 2
#define ARG_QUIET                   3
#define ARG_INMEMORY                10
#define ARG_OVERWRITE               11
#define ARG_PROGRESS                12
#define ARG_DISABLE_MMAP            13
#define ARG_PAGESIZE                14
#define ARG_KEYSIZE                 15
#define ARG_CACHESIZE               16
#define ARG_REOPEN                  18
#define ARG_USE_CURSORS             23
#define ARG_NUMERIC                 24
#define ARG_DUPLICATE               26
#define ARG_FULLCHECK_FIND          27
#define ARG_FULLCHECK_BACKWARDS     28
#define ARG_DUPLICATE_FIRST         31
#define ARG_RECOVERY                34
#define ARG_CACHEUNLIMITED          35
#define ARG_HINTING                 37
#define ARG_DIRECT_ACCESS           39
#define ARG_USE_TRANSACTIONS        41
#define ARG_WRITETHROUGH            42
#define ARG_NO_BDB                  43
#define ARG_NUM_THREADS             44
#define ARG_ENABLE_ENCRYPTION       45
#define ARG_ENABLE_REMOTE           46

Metrics *Metrics::instance;

/*
 * command line parameters
 */
static option_t opts[]={
  { 
    ARG_HELP,         // symbolic name of this option
    "h",          // short option 
    "help",         // long option 
    "this help screen",   // help string
    0 },          // no flags
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
    ARG_HINTING,
    0,
    "hints",
    "sets hinting flags - one of:\n"
    "\tHAM_HINT_APPEND, HAM_HINT_PREPEND",
    GETOPTS_NEED_ARGUMENT },
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
  {
    ARG_ENABLE_ENCRYPTION,
    0,
    "enable-encryption",
    "enables AES encryption",
    0 },
  {
    ARG_ENABLE_REMOTE,
    0,
    "enable-remote",
    "runs test in remote client/server scenario",
    0 },
  { 0, 0, 0, 0, 0 }
};

static void
parse_config(int argc, char **argv, Configuration *c)
{
  unsigned opt;
  char *param;
	
  getopts_init(argc, argv, "test");

  /*
   * parse command line parameters
   */
  while ((opt = getopts(&opts[0], &param))) {
    if (opt == ARG_HELP) {
      getopts_usage(&opts[0]);
      exit(0);
    }
    else if (opt == ARG_QUIET) {
      c->quiet = true;
    }
    else if (opt == ARG_VERBOSE) {
      c->verbose++;
    }
    else if (opt == ARG_INMEMORY) {
      c->inmemory = true;
    }
    else if (opt == ARG_OVERWRITE) {
      if (c->duplicate) {
        printf("invalid combination: overwrite && duplicate\n");
        exit(-1);
      }
      c->overwrite = true;
    }
    else if (opt == ARG_DUPLICATE) {
      if (c->overwrite) {
        printf("invalid combination: overwrite && duplicate\n");
        exit(-1);
      }
      c->duplicate = true;
    }
    else if (opt==ARG_DUPLICATE_FIRST) {
      if (!c->use_cursors) {
        printf("'duplicate-first' needs 'use-cursors'\n");
        exit(-1);
      }
      c->dupe_flags = HAM_DUPLICATE_INSERT_FIRST;
      c->duplicate = true;
    }
    else if (opt == ARG_USE_CURSORS) {
      c->use_cursors = true;
    }
    else if (opt == ARG_RECOVERY) {
      c->recovery = true;
    }
    else if (opt == ARG_NUMERIC) {
      c->set_numeric(true);
    }
    else if (opt == ARG_PROGRESS) {
      c->progress = true;
    }
    else if (opt == ARG_DISABLE_MMAP) {
      c->no_mmap = true;
    }
    else if (opt == ARG_PAGESIZE) {
      c->pagesize = strtoul(param, 0, 0);
    }
    else if (opt == ARG_KEYSIZE) {
      c->keysize = strtoul(param, 0, 0);
    }
    else if (opt == ARG_CACHESIZE) {
      c->cachesize = strtoul(param, 0, 0);
    }
    else if (opt == ARG_CACHEUNLIMITED) {
      c->cacheunlimited = true;
    }
    else if (opt == ARG_HINTING) {
      if (strstr(param, "HAM_HINT_APPEND"))
        c->hints |= HAM_HINT_APPEND;
      if (strstr(param, "HAM_HINT_PREPEND"))
        c->hints |= HAM_HINT_PREPEND;
      if (param && !c->hints) {
        printf("invalid or missing parameter for 'hints'\n");
        exit(-1);
      }
    }
    else if (opt == ARG_DIRECT_ACCESS) {
      c->direct_access = true;
    }
    else if (opt == ARG_WRITETHROUGH) {
      c->use_writethrough = true;
    }
    else if (opt == ARG_NO_BDB) {
      c->no_bdb = true;
    }
    else if (opt == ARG_USE_TRANSACTIONS) {
      c->enable_transactions = true;
      if (strcmp("tmp", param) == 0)
        c->txn_group = 0;
      else if (strcmp("all", param) == 0)
        c->txn_group = 0xffffffff;
      else {
        c->txn_group = strtoul(param, 0, 0);
        if (!c->txn_group) {
          printf("invalid parameter for 'use-transactions'\n");
          exit(-1);
        }
      }
    }
    else if (opt == ARG_REOPEN) {
      c->reopen = true;
    }
    else if (opt == ARG_FULLCHECK_FIND) {
      c->fullcheck_find = true;
    }
    else if (opt == ARG_FULLCHECK_BACKWARDS) {
      c->fullcheck_backwards = true;
    }
    else if (opt == ARG_NUM_THREADS) {
      c->num_threads = strtoul(param, 0, 0);
      if (!c->num_threads) {
        printf("invalid parameter for 'num-threads'\n");
        exit(-1);
      }
    }
    else if (opt == ARG_ENABLE_ENCRYPTION) {
      c->encryption = true;
    }
    else if (opt == ARG_ENABLE_REMOTE) {
      c->remote = true;
    }
    else if (opt == GETOPTS_PARAMETER) {
      c->filename = param;
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
  Configuration c;
  parse_config(argc, argv, &c);
  Metrics::get_instance();

  Parser p(&c, c.filename);
  Controller ctrl(&c, p);

  unsigned i = 0;
  bool ok = true;

  std::vector<Thread *> threads;
  for (i = 0; i < c.num_threads; i++)
    threads.push_back(new Thread(i + 1, ctrl, c, p, "hamsterdb"));
  if (!c.no_bdb)
    threads.push_back(new Thread(i + 1, ctrl, c, p, "berkeleydb"));

  ctrl.run(threads);

  for (i = 0; i < threads.size(); i++) {
    threads[i]->join();
    if (!threads[i]->success())
      ok = false;
  }
  for (i = 0; i < threads.size(); i++)
    delete threads[i];
  threads.clear();

  if (c.progress)
    printf("\n");
  if (ok)
    printf("[OK] %s\n", c.filename);
  else
    printf("[FAIL] %s\n", c.filename);

  if (ok && !c.quiet)
    Metrics::get_instance()->print(&c);

  delete Metrics::get_instance();

  return (ok ? 0 : 1);
}
