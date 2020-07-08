#include <iostream>
#include <limits>
#include <string>
#include <exception>
#include <sstream>
#include <typeinfo>
#include <type_traits>
#include <set>

using namespace std;
#define SHOW0(comment) { cout << (comment) << endl; }
#define SHOW(expr, comment) { cout << "line(" << __LINE__ << "): " << boolalpha << (#expr)\
                                  << "=" << (expr) << "\t\t" << (comment) << endl; }

constexpr int64_t const MICRO_RAD_PI  = 3'141'592LL;
constexpr int64_t const MICRO_RAD_2PI = MICRO_RAD_PI*2 + 1;

#define STEP 4

#define SHOW_COMPILE_ERROR

#if STEP == 0                    // no template
#include "limited_int_0.h" 
#endif
#if STEP == 1                    // Don't do that - naive");
#include "limited_int_1.h" 
#endif
#if STEP == 2                    // Basic limited int - use modulo operation to get into range
#include "limited_int_2.h" 
#endif
#if STEP == 3                    // better output, conversion
#include "limited_int_3.h"
#endif
#if STEP == 4                    // Make sure that template parameters are sane
#include "limited_int_4.h"
#endif
#if STEP == 5                    // illegal values -> policy
#include "limited_int_5.h"
#endif
#if STEP == 6                    // exceptions-policy + more template param sanity
#include "limited_int_6.h"       
#endif
#if STEP == 7
#include "extras.h"
#endif

int main(int argc, char** argv)
{
   execute();
   
   return 0;
}

