# automake input file
# Note: automake for this project is no longer maintained
#       use 'cmake' instead
#       Last tested 01/09/2023
#       
#       BOOST - path hard-coded and not auto-detected

AUTOMAKE_ARGS = --add-missing

lib_LIBRARIES = libdkutil.a
libutil_a_SOURCES = src/anyutil.cc \
					src/anyutil.cc \
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

check_PROGRAMS	= run_tests

run_tests_SOURCES = test/run_tests.cc \
		    		test/anyutil_tests.cc \
		    		test/bayesutil_tests.cc \
		    		test/bit_converter_tests.cc \
		    		test/brackets_tests.cc \
		    		test/container_convert_tests.cc \
		    		test/csvutil_tests.cc \
		    		test/dateutil_tests.cc \
		    		test/decorator_tests.cc \
		    		test/FFT_tests.cc \
		    		test/graphutil_tests.cc \
		    		test/instance_pool_tests.cc \
		    		test/limited_int_tests.cc \
		    		test/logval_tests.cc \
		    		test/matrix_tests.cc \
		    		test/primes_tests.cc \
		    		test/statutil_tests.cc \
		    		test/stringutil_tests.cc \
		    		test/tinytea_tests.cc

LDADD = $(top_builddir)/libdkutil.a /usr/local/lib/libboost_filesystem.a
run_tests_LDADD =${LDADD} /usr/local/lib/libgtest.a /usr/local/lib/libgtest_main.a /usr/local/lib/libgmp.a

TESTS	= run_tests
