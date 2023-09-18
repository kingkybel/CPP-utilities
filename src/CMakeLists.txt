add_library(dkutils STATIC
  anyutil.cc
  bayesutil.cc
  csvutil.cc
  dateutil.cc
  floatingpoint.cc
  primes.cc
  statutil.cc
  threadutil.cc
)

target_link_libraries(dkutils PRIVATE gmp boost_filesystem)
