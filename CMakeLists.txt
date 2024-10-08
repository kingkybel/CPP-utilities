cmake_minimum_required(VERSION 3.8)

if(CMAKE_BUILD_TYPE STREQUAL "")
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

message("BUILD TYPE " ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Debug Release RelWithDebInfo MinSizeRel Coverage")
set(DISPLAY_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel;Coverage" CACHE STRING "Configs" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)

execute_process(
        COMMAND bash -c "printf \"%s\" $(cat /etc/os-release | grep -E \"^ID=[^\\n]*|^VERSION_ID=[^\\n]*\" | cut -f2 -d= | tr -d '\"')"
        OUTPUT_VARIABLE LINUX_VERSION
)
message("Linux version is '${LINUX_VERSION}'")

include(cmake/find_static_lib.cmake)

set(COVERAGE_COMPILER_FLAGS "-g -O0 -fno-default-inline --coverage -fprofile-abs-path -fprofile-arcs -fno-inline -fno-inline-small-functions -ftest-coverage -lgcov" CACHE INTERNAL "")

set(CMAKE_CXX_FLAGS "-rdynamic -Wall -Werror -DBOOST_ALLOW_DEPRECATED_HEADERS -DBOOST_BIND_GLOBAL_PLACEHOLDERS" CACHE STRING "Default C++ compiler flags")
set(CMAKE_CXX_FLAGS_DEBUG "-g" CACHE STRING "g++ debug flags")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2" CACHE STRING "g++ rel+gbg flags")
set(CMAKE_CXX_FLAGS_COVERAGE "${COVERAGE_COMPILER_FLAGS}" CACHE STRING "g++ coverage flags." FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O2" CACHE STRING "g++ release flags")
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_FIND_LIBRARY_PREFIXES lib)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

# include(FetchContent)
# FetchContent_Declare(
# googletest
# # Specify the commit you depend on and update it regularly.
# URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
# )
# set(FETCHCONTENT_QUIET OFF)
# For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

# include(FetchContent)
# FetchContent_Declare(
# Boost
# GIT_REPOSITORY https://github.com/boostorg/boost.git
# GIT_TAG boost-1.83.0
# )
# FetchContent_MakeAvailable(Boost)
find_library(DL_LIB NAMES libdl.a dl)
find_library(SSL_LIB NAMES libssl.a ssl)
find_library(PTHREAD_LIB NAMES libpthread.a pthread)
find_package(Boost 1.86.0 REQUIRED)
message("Boost_INCLUDE_DIRS '${Boost_INCLUDE_DIRS}'")

project(DKUtil CXX)
include_directories(
        ${Boost_INCLUDE_DIRS}
        BEFORE SYSTEM ${CMAKE_SOURCE_DIR}/include
        system
)
link_directories(
        BEFORE SYSTEM ${CMAKE_SOURCE_DIR}/build/lib
        /usr/local/lib
        ${Boost_LIBRARY_DIR_RELEASE}
        system
)

add_subdirectory(src)
add_subdirectory(test)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=* -p ${CMAKE_SOURCE_DIR}/build")

file(GLOB HEADER_FILES ${CMAKE_SOURCE_DIR}/include/*.h)

# Install the header files directly under 'include/dkyb'
install(FILES ${HEADER_FILES}
        DESTINATION include/dkyb)

install(TARGETS dkanyutil dkcsvutil dkdateutil dkthreadutil
        LIBRARY DESTINATION lib # For shared libraries (.so)
        ARCHIVE DESTINATION lib # For static libraries (.a)
        RUNTIME DESTINATION bin # For executables
)
