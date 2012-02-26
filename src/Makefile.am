
CXXFLAGS = $(CFLAGS)

noinst_PROGRAMS	= test

INCLUDES  = -I$(top_builddir)/env/common

AM_CFLAGS =

# debug or release mode? - the test program is always compiled in 
# debug mode, because it contains a lot of asserts and debug output
AM_CFLAGS += -DDEBUG -DHAM_DEBUG

# use profiling?
if PROFILE
    AM_CFLAGS += -pg
endif

test_SOURCES    = parser.cpp engine.cpp main.cpp getopts.c hamsterdb.cpp \
				  berkeleydb.cpp os.cpp database.cpp
test_LDFLAGS    = 
test_LDADD      = $(top_builddir)/env/posix/libhamsterdb.a \
				  -ldb -lpthread -lcurl -lprotobuf
