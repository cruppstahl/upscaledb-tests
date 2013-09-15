
noinst_PROGRAMS	= test

AM_CPPFLAGS = -I$(top_builddir)/env/common -I/usr/local/BerkeleyDB.5.2/include

test_SOURCES    = configuration.h \
				  datasource.h \
				  datasource_binary.h \
				  datasource_numeric.h \
				  generator.h \
				  generator_runtime.h \
				  generator_runtime.cc \
				  getopts.h \
				  getopts.c \
				  main.cc

test_LDFLAGS    = 
test_LDADD      = $(top_builddir)/env/posix/libhamsterdb.a \
                  $(top_builddir)/env/posix/libhamserver.a \
				  -ldb-5.2 -L/usr/local/BerkeleyDB.5.2/lib -lcrypto \
				  $(BOOST_CHRONO_LIBS) $(BOOST_THREAD_LIBS)
				  -lpthread -luv -lprotobuf -ltcmalloc_minimal
