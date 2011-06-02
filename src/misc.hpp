
#ifndef MISC_HPP__
#define MISC_HPP__

#include <stdio.h>

#define VERBOSE(x)  while (m_config->verbose) { printf x; break; }
#define TRACE(x)    printf x

#endif /* MISC_HPP__ */
