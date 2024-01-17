# add_library(dkutils STATIC
#   anyutil.cc
#   bayesutil.cc
#   csvutil.cc
#   dateutil.cc
#   floatingpoint.cc
#   primes.cc
#   statutil.cc
#   threadutil.cc
# )

add_library(dkanyutil STATIC anyutil.cc)
add_library(dkbayesutil STATIC bayesutil.cc)
add_library(dkcsvutil STATIC csvutil.cc)
add_library(dkdateutil STATIC dateutil.cc)
add_library(dkfloatingpoint STATIC floatingpoint.cc)
find_static_library(boost_filesystem BOOST_FILESYSTEM)
target_link_libraries(dkfloatingpoint PRIVATE gmp ${BOOST_FILESYSTEM})
add_library(dkprimes STATIC primes.cc)
add_library(dkstatutil STATIC statutil.cc)
add_library(dkthreadutil STATIC threadutil.cc)


# target_link_libraries(dkutils PRIVATE gmp boost_filesystem)
