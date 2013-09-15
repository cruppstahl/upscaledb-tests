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


#include <iostream>
#include <cstdio>

#include "getopts.h"
#include "configuration.h"
#include "datasource.h"
#include "datasource_numeric.h"
#include "datasource_binary.h"
#include "generator_runtime.h"

#define ARG_HELP                    1
#define ARG_VERBOSE                 2
#define ARG_QUIET                   3
#define ARG_PROGRESS                4

/*
 * command line parameters
 */
static option_t opts[] = {
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
    ARG_PROGRESS,
    "p",
    "progress",
    "show progress",
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
    else if (opt == ARG_PROGRESS) {
      c->progress = true;
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
  bool ok = true;

  //NumericDescendingDatasource<unsigned short> ds;
  //NumericRandomDatasource<unsigned short> ds;
  //NumericZipfianDatasource<unsigned int> ds(100);
  std::vector<unsigned char> t;
  RuntimeGenerator generator(&c);
  //BinaryRandomDatasource ds(5, false);
  BinaryZipfianDatasource ds(100, 5, false);
  while (generator.execute(0)) {
    t.clear();
    std::cout.width(2);
    std::cout.fill('0');
    ds.get_next(t);
    for (size_t j = 0; j < t.size(); j++)
      printf("0x%02x ", t[j]);
    std::cout << std::endl;
  }

  if (c.progress)
    printf("\n");
  if (ok)
    printf("[OK] %s\n", c.filename);
  else
    printf("[FAIL] %s\n", c.filename);

  return (ok ? 0 : 1);
}
