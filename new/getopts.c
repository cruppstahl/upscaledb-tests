/* 
 * getopts()

  Copyright (C) 2005-2012 Christoph Rupp, www.crupp.de

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

*/

#include <stdio.h>
#include <string.h>
#include "getopts.h"

static int cur = 0;
static int argc = 0;
static char **argv = 0;
static const char *program = 0;

void
getopts_init(int argc_, char **argv_, const char *program_)
{
  cur = 0;
  argc = argc_ - 1;
  argv = argv_ + 1;
  program = program_;
}

void
getopts_usage(option_t *options)
{
  printf("usage: %s <options>\n", program);
  for (; options->longopt; options++) {
    if (options->flags & GETOPTS_NEED_ARGUMENT) {
      if (options->shortopt)
        printf("  -%s, --%s:arg: %s\n", 
                options->shortopt, options->longopt, options->helpdesc);
      else
        printf("  --%s:arg: %s\n", 
                options->longopt, options->helpdesc);
    }
    else {
      if (options->shortopt)
        printf("  -%s, --%s: %s\n", 
                options->shortopt, options->longopt, options->helpdesc);
      else
        printf("  --%s: %s\n", 
                options->longopt, options->helpdesc);
    }
  }
  printf("\n");
}

unsigned int 
getopts(option_t *options, char **param)
{
  char *p;
  option_t *o = options;

  if (!argv)
    return (GETOPTS_NO_INIT);

  if (cur >= argc || argv[cur] == 0)
    return (0);

  /*
   * check for a long option with --
   */
  if (argv[cur ][0] == '-' && argv[cur ][1] == '-') {
    *param = &argv[cur][2];
    for (; o->longopt; o++) {
      int found = 0;
      if (!strcmp(o->longopt, &argv[cur][2]))
        found = 1;
      else if (strstr(&argv[cur][2], o->longopt) == &argv[cur][2]) {
        if (argv[cur][2 + strlen(o->longopt)] == ':')
          found = 1;
        else if (argv[cur][2 + strlen(o->longopt)] == '=')
          found = 1;
      }
      if (found) {
        if (o->flags & GETOPTS_NEED_ARGUMENT) {
          p = strchr(&argv[cur][1], ':');
          if (p) {
            *param = p + 1;
            cur++;
            return (o->name);
          }
          p = strchr(&argv[cur][1], '=');
          if (p) {
            *param = p + 1;
            cur++;
            return (o->name);
          }
          if (cur == argc)
            return (GETOPTS_MISSING_PARAM);
          *param = argv[cur + 1];
          cur++;
        }
        cur++;
        return (o->name);
      }
    }
    return (GETOPTS_UNKNOWN);
  }

  /*
   * check for a short option name
   */
  else if (argv[cur][0] == '-') {
    *param = &argv[cur][1];
    for (; o->longopt; o++) {
      if (!o->shortopt)
        continue;
      if (!strcmp(o->shortopt, &argv[cur][1])) {
        if (o->flags & GETOPTS_NEED_ARGUMENT) {
          if (cur == argc)
            return (GETOPTS_MISSING_PARAM);
          *param = argv[cur + 1];
          cur++;
        }
        cur++;
        return (o->name);
      }
    }
    return (GETOPTS_UNKNOWN);
  }

  if (param)
    *param = argv[cur];
  cur++;
  return (GETOPTS_PARAMETER);
}
