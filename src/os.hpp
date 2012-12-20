/**
 * Copyright (C) 2005-2012 Christoph Rupp (chris@crupp.de).
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * See files COPYING.* for License information.
 */


#ifndef OS_HPP__
#define OS_HPP__

#include <ham/types.h>

class os
{
  public:
    static bool unlink(const char *path);

    static ham_u64_t now() {
#ifdef WIN32
      ham_u64_t usec, sec;
      union {
        __int64 ns100; /*time since 1 Jan 1601 in 100ns units */
        FILETIME ft;
      } now;
      GetSystemTimeAsFileTime(&now.ft);
      usec = (long) ((now.ns100 / 10LL) % 1000000LL);
      sec = (long) ((now.ns100 - 116444736000000000LL) / 10000000LL);
	  return (ham_u64_t)(sec) * 1000 + (ham_u64_t)(usec) / 1000;
#else
	  struct timeval tv;
      gettimeofday(&tv, 0);
      return (ham_u64_t)(tv.tv_sec) * 1000 + (ham_u64_t)(tv.tv_usec) / 1000;
#endif
    }

    static unsigned long get_filesize(const char *path);
};

#endif /* OS_HPP__ */
