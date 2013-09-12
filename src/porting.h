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


#ifndef PORTING_HPP__
#define PORTING_HPP__

#include <ham/types.h>
#include <ham/hamsterdb_int.h> /* for DAM stuff */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#ifdef WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#  include <sys/time.h>
#  include <sys/utsname.h>
#endif

#if WIN32
#  define strcasecmp stricmp
#endif

#endif /* PORTING_HPP__ */
