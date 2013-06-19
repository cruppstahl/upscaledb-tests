
CXXFLAGS = $(CFLAGS)

noinst_PROGRAMS	= test

INCLUDES  = -I$(top_builddir)/env/common

# debug or release mode? - the test program is always compiled in 
# debug mode, because it contains a lot of asserts and debug output
AM_CXXFLAGS = -DDEBUG -DHAM_DEBUG -I/usr/local/BerkeleyDB.5.2/include

test_SOURCES    = parser.cpp engine.cpp main.cpp getopts.c hamsterdb.cpp \
				  controller.cpp berkeleydb.cpp os.cpp database.cpp
test_LDFLAGS    = 
test_LDADD      = $(top_builddir)/env/posix/libhamsterdb.a \
				  -ldb-5.2 -L/usr/local/BerkeleyDB.5.2/lib \
				  -lboost_thread -lpthread -lcurl -lprotobuf -ltcmalloc_minimal
