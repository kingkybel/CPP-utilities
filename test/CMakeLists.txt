add_executable(run_tests
                run_tests.cc
                bit_converter_tests.cc
                limited_int_tests.cc
                stringutil_tests.cc
                dateutil_tests.cc
                anyutil_tests.cc
                container_convert_tests.cc
                brackets_tests.cc
                decorator_tests.cc
                csvutil_tests.cc
                graphutil_tests.cc
                matrix_tests.cc
                instance_pool_tests.cc
                logval_tests.cc
                tinytea_tests.cc
                FFT_tests.cc
                primes_tests.cc
                statutil_tests.cc
                bayesutil_tests.cc)

target_link_libraries(run_tests gtest gtest_main gmp dkutils)
add_test(NAME run_tests COMMAND run_tests)
include(Dart)

