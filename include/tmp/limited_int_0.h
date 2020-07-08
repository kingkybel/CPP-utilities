#ifndef LIMITED_INT_H_INCLUDED
#define LIMITED_INT_H_INCLUDED


void execute()
{
   SHOW0("================ NO TEMPLATES ================")
   #define SQUARE(a) ((a)*(a))
   int x = 1;
   SHOW(x,"");
   x = SQUARE(++x);
   SHOW(x,"");
   SHOW(SQUARE(++x),"");
   
   SHOW0("------------------ NATIVE P.O.D. types ----------")
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

