/**
 * Copyright (C) 2005-2008 Christoph Rupp (chris@crupp.de).
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * See files COPYING.* for License information.
 */

#include <stdexcept>
#include <cstdlib>
#include <cstring>

#include <ham/hamsterdb.h>
#include "memtracker.hpp"

unsigned long TrackingAllocator::ms_mem_allocs;
unsigned long TrackingAllocator::ms_mem_peak;
unsigned long TrackingAllocator::ms_mem_current;
unsigned long TrackingAllocator::ms_mem_total;
boost::mutex TrackingAllocator::ms_mutex;

