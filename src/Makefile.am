
noinst_PROGRAMS	= test

AM_CPPFLAGS =

# debug or release mode? - the test program is always compiled in 
# debug mode, because it contains a lot of asserts and debug output
AM_CXXFLAGS = -DDEBUG -DHAM_DEBUG

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
test_LDADD      = -lhamsterdb -lhamserver -lcrypto \
				  -lboost_thread -lpthread -luv -lprotobuf -ltcmalloc_minimal
