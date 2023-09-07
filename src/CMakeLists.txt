add_library(dkutils STATIC
  anyutil.cc
  csvutil.cc
  dateutil.cc
  floatingpoint.cc
  primes.cc
  statutil.cc
  bayesutil.cc
)

target_link_libraries(dkutils PRIVATE gmp boost_filesystem)
