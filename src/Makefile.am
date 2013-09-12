
noinst_PROGRAMS	= test

AM_CPPFLAGS = -I$(top_builddir)/env/common

# debug or release mode? - the test program is always compiled in 
# debug mode, because it contains a lot of asserts and debug output
AM_CXXFLAGS = -DDEBUG -DHAM_DEBUG -I/usr/local/BerkeleyDB.5.2/include

test_SOURCES    = berkeleydb.cc \
				  berkeleydb.h \
				  configuration.h \
				  controller.cc \
				  controller.h \
				  database.cc \
				  database.h \
				  endianswap.h \
				  engine.cc \
				  engine.h \
				  getopts.h \
				  getopts.c \
				  hamsterdb.cc \
				  hamsterdb.h \
				  main.cc \
				  metrics.h \
				  misc.h \
				  os.cc \
				  os.h \
				  parser.cc \
				  parser.h \
				  porting.h \
				  thread.h

test_LDFLAGS    = 
test_LDADD      = $(top_builddir)/env/posix/libhamsterdb.a \
                  $(top_builddir)/env/posix/libhamserver.a \
				  -ldb-5.2 -L/usr/local/BerkeleyDB.5.2/lib -lcrypto \
				  -lboost_thread -lpthread -luv -lprotobuf -ltcmalloc_minimal
