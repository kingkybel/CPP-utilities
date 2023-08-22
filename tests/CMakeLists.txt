set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(util_tests
                testrunner.cc 
                anyutilTest.cc 
                bayesutilTest.cc 
                csvutilTest.cc 
                dateutilTest.cc 
                FFTTest.cc 
                graphutilTest.cc 
                instancePoolTest.cc 
                limitedIntTest.cc 
                logValTest.cc 
                matrixTest.cc 
                primesTest.cc 
                statutilTest.cc 
                stringutilTest.cc 
                tinyTeaTest.cc
)

target_link_libraries(util_tests PRIVATE boost_filesystem cppunit dkutils)
