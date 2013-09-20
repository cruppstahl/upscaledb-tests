
noinst_PROGRAMS	= test

if WITH_BERKELEYDB
AM_CPPFLAGS     = -DHAM_WITH_BERKELEYDB
endif

test_SOURCES    = berkeleydb.h \
				  berkeleydb.cc \
				  configuration.h \
				  database.h \
				  database.cc \
				  datasource.h \
				  datasource_binary.h \
				  datasource_numeric.h \
				  generator.h \
				  generator_runtime.h \
				  generator_runtime.cc \
				  getopts.h \
				  getopts.c \
				  hamsterdb.h \
				  hamsterdb.cc \
				  main.cc

test_LDADD      = -lhamsterdb -lhamserver -lcrypto \
				  $(BOOST_CHRONO_LIBS) $(BOOST_SYSTEM_LIBS) \
				  $(BOOST_THREAD_LIBS) $(BOOST_FILESYSTEM_LIBS) \
				  -lpthread -luv -lprotobuf -ltcmalloc_minimal
