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


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "porting.hpp"
#include "os.hpp"


bool 
os::unlink(const char *path)
{
#ifdef WIN32
  return (0 != DeleteFile(path));
#else
  return (0 == ::unlink(path));
#endif
}

unsigned long 
os::get_filesize(const char *path)
{
  struct stat buf;
  if (stat(path, &buf) < 0)
    return (0);
  return ((unsigned long)buf.st_size);
}
