add_library(dkutils STATIC
  anyutil.cc
  stringutil.cc
  csvutil.cc
  dateutil.cc
  graphutil.cc
  floatingpoint.cc
  limited_int.cc
  primes.cc
  statutil.cc
  bayesutil.cc
)

target_link_libraries(dkutils PRIVATE gmp boost_filesystem)
