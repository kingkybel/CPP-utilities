#ifndef LIMITED_INT_H_INCLUDED
#define LIMITED_INT_H_INCLUDED

#include <iostream>
#include <limits>
#include <exception>
#include <sstream>

using namespace std;

#define SHOW(expr, comment) {cout << "line("<< __LINE__ << "): " << boolalpha << (#expr)\
                                  << "=" << (expr) << "\t\t" << (comment) << endl; }

void execute()
{

   int32_t deg360 = 270;         // ok
   SHOW(deg360, "valid");
   
   int32_t deg180 = -90;         // ok
   SHOW(deg180, "valid");
   
   int32_t rad2Pi = 1'234'567;   // ok
   SHOW(rad2Pi, "valid");
   
   deg360 = 510;  // we don't want values like that!
   SHOW(deg360, "value >= 360");
   
   deg360 = (510 % 360); // [0, 359]
   SHOW(deg360, "valid after modulo");
   
   deg360 = 359;
   deg180 = deg360; // > 180!
   SHOW(deg180, "after assignment of 360 value to 180 value");
   deg180 = (deg360 > 179 ? ((deg360 % 360) - 360) : (deg360 % 360));
   
   SHOW(deg180, "after conversion");
   //...
}


#endif // LIMITED_INT_H_INCLUDED

