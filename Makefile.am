AUTOMAKE_ARGS = --add-missing

lib_LIBRARIES = libutil.a
libutil_a_SOURCES = src/anyutil.cc \
		    src/bayesutil.cc \
		    src/csvutil.cc \
		    src/dateutil.cc \
		    src/floatingpoint.cc \
		    src/graphutil.cc \
		    src/limited_int.cc \
		    src/primes.cc \
		    src/statutil.cc \
		    src/stringutil.cc
AM_CPPFLAGS = -I ./include -std=c++20
ACLOCAL_AMFLAGS = -I /usr/local/share/aclocal

check_PROGRAMS	= testrunner

testrunner_SOURCES = tests/testrunner.cc \
		    tests/anyutilTest.cc \
		    tests/bayesutilTest.cc \
		    tests/csvutilTest.cc \
		    tests/dateutilTest.cc \
		    tests/FFTTest.cc \
		    tests/graphutilTest.cc \
		    tests/instancePoolTest.cc \
		    tests/limitedIntTest.cc \
		    tests/logValTest.cc \
		    tests/matrixTest.cc \
		    tests/primesTest.cc \
		    tests/statutilTest.cc \
		    tests/stringutilTest.cc \
		    tests/tinyTeaTest.cc

LDADD = $(top_builddir)/libutil.a $(BOOST_FILESYSTEM_LIB)
testrunner_LDADD =${LDADD} /usr/local/lib/libcppunit.so

TESTS	= testrunner
