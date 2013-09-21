dnl
dnl configuration script for hamsterdb-tests
dnl
dnl Copyright (C) 2005-2013 Christoph Rupp (chris@crupp.de).
dnl All rights reserved. See file LICENSE for licence and copyright
dnl information
dnl

dnl Initialize autoconf/automake
AC_INIT([hamsterdb-tests], [0.0.1], [http://hamsterdb.com])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])
AC_CONFIG_MACRO_DIR(m4)
AC_COPYRIGHT([Copyright (C) 2005-2013 Christoph Rupp (chris@crupp.de)])
AC_CANONICAL_HOST
AC_ENABLE_SHARED
AC_ENABLE_STATIC
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_SED
AC_HEADER_STDC
AC_C_CONST
AC_TYPE_SIZE_T
AC_SYS_LARGEFILE
# AM_PROG_AR is not available in automake v0.11 but it's required in v0.12
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
LT_INIT
AC_CONFIG_SRCDIR(src/hamsterdb.cc)
AC_CONFIG_HEADERS(config.h)
AC_CHECK_FUNCS(mmap munmap getpagesize fdatasync fsync writev pread pwrite)
AC_CHECK_HEADERS(fcntl.h unistd.h malloc.h uv.h db.h)
AC_TYPE_OFF_T
AC_FUNC_MMAP

m4_include([m4/boost.m4])
BOOST_REQUIRE([1.36])
BOOST_SYSTEM()
BOOST_CHRONO()
BOOST_FILESYSTEM()
BOOST_THREADS()

AM_CONDITIONAL([DARWIN],  [AS_CASE([$host_os], [darwin*],  [true], [false])])
AM_CONDITIONAL([FREEBSD], [AS_CASE([$host_os], [freebsd*], [true], [false])])
AM_CONDITIONAL([LINUX],   [AS_CASE([$host_os], [linux*],   [true], [false])])
AM_CONDITIONAL([SUNOS],   [AS_CASE([$host_os], [solaris*], [true], [false])])

# A string describing all enabled/disabled settings
settings=""

# -------------------------------------------------------------------------
# Enable debug mode? This switch is not recommended according to autoconf
# philosophies, but people are used to have it.
# -------------------------------------------------------------------------
AC_ARG_ENABLE(debug,
  AS_HELP_STRING(--enable-debug, build for debugging (slow!)),
    [if test x$enableval = xyes; then
      CFLAGS="-g -O0 -DHAM_DEBUG -Wall -pedantic"
      settings="$settings (debug)"
     fi])

# -------------------------------------------------------------------------
# Berkeleydb header files and libraries
# -------------------------------------------------------------------------
if test "x$ac_cv_header_db_h" = "xyes"; then
  AC_CHECK_LIB(db, db_create)
  AM_CONDITIONAL(WITH_BERKELEYDB, test "x$ac_cv_lib_db_db_create" = "xyes")
  if test "x$ac_cv_lib_db_db_create" = "xyes"; then
    settings="$settings (berkeleydb)"
  else
    settings="$settings (without berkeleydb)"
  fi
else
  AM_CONDITIONAL(WITH_BERKELEYDB, false)
  settings="$settings (without berkeleydb)"
fi

# -------------------------------------------------------------------------
# The path of the hamsterdb library
# -------------------------------------------------------------------------
if test "x$hamsterdb" = x; then
  echo "hamsterdb directory not specified; run ./configure hamsterdb=<dir>"
  exit -1
else
  settings="$settings ($hamsterdb)"
  CPPFLAGS="-I$hamsterdb/include"
  LDFLAGS="-L$hamsterdb/src/.libs -L$hamsterdb/src/server/.libs"
fi

CXXFLAGS=${CFLAGS}

# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
AC_CONFIG_FILES(Makefile src/Makefile new/Makefile)
AC_OUTPUT

printf '#================================================================\n'
printf "# Configuring hamsterdb-tests version $PACKAGE_VERSION\n"
printf "# Settings:$settings\n"
printf "\n"

