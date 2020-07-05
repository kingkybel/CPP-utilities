
#include <cstdlib>


#define STEP 6

//#define SHOW_COMPILE_ERROR

#if STEP == 0                    // no template
#include "limited_int_0.h"
#endif
#if STEP == 1                    // Basic limited int - use modulo operation to get into range
#include "limited_int_1.h"
#endif
#if STEP == 2                    // better output, conversion
#include "limited_int_2.h"
#endif
#if STEP == 3                    // Make sure that template parameters are sane
#include "limited_int_3.h"
#endif
#if STEP == 4                    // illegal values -> policy
#include "limited_int_4.h"
#endif
#if STEP == 5                    // exceptions-policy + more template param sanity
#include "limited_int_5.h"
#endif
#if STEP == 6
#include "limited_int_6.h"
#endif
#if STEP == 7
#include <limited_int_7.h>
#endif
#if STEP == 8
#include <limited_int_8.h>
#endif

int main(int argc, char** argv)
{
    execute();

    return 0;
}

