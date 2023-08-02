add_library(dkutils STATIC
  anyutil.cc
  bayesutil.cc
  csvutil.cc
  dateutil.cc
  floatingpoint.cc
  graphutil.cc
  iosutil.cc
  limited_int.cc
  primes.cc
  statutil.cc
  stringutil.cc
)

target_link_libraries(dkutils PRIVATE boost_string boost_gregorian)
