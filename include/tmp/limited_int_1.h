#ifndef LIMITED_INT_H_INCLUDED
#define LIMITED_INT_H_INCLUDED

template <typename T_>
struct limited_int
{
private:
   const T_ min_; // @NOTE: min_ and max_ const members
   const T_ max_;
   T_ val_ = min_;

public:

   limited_int(T_ val, T_ const min, T_ const max) // @NOTE: no default constructor because min/max
   : min_(min)
   , max_(max)
   , val_(val)
   {
      if ((val > max_) || (val < min_))
      {
         T_ const dist = max_ - min_ + 1;
         val -= min_;
         val %= dist;
         if (val < 0)
            val += dist;
         val += min_;
      }
      val_ = val;
   }
   
   // @NOTE: Assignment operator from native type 
   limited_int& operator = (T_ rhs)
   {
      limited_int other(rhs, min_, max_);
      val_ = other.val_;
   }

   limited_int min() const // @NOTE:  const member functions
   {
      return min_;
   }

   limited_int max() const // @NOTE:  const member functions
   {
      return max_;
   }

   T_ val() const 
   {
      return val_;
   }

   operator T_() const // NON - explicit! cast operator
   {
      return val();
   }

};

typedef limited_int<long> Deg180;
typedef limited_int<long> Deg360;
typedef limited_int<long> Rad2Pi;

void execute()
{
   SHOW0("================ NAIVE TEMPLATE - TYPE ONLY ================")
   Deg360 deg360 = {270, 0, 359};         // ok - but ugly
   SHOW(deg360, "valid");
   deg360 = 510;
   SHOW(deg360, "valid");
   
   Deg180 deg180 = {-90, -180, 180};        // ok - but ugly
   SHOW(deg180, "valid");

   SHOW0("--------- SIZEOF ---------");
   SHOW(sizeof(Deg360),"");
   SHOW(sizeof(long),"");
   SHOW0("--------------------------");
   
   Rad2Pi rad2Pi = {1'234'567, 0, MICRO_RAD_2PI};   // ok - but ugly
   SHOW(rad2Pi, "valid");
   
   deg360 = 510;  // we don't want values like that!
   SHOW(deg360, "now has a valid value");
   
   deg360 = (510 % 360); // [0, 359]
   SHOW(deg360, "valid after modulo REDUNDANT");
   
   deg360 = 359;
   
#ifdef SHOW_COMPILE_ERROR
   deg180 = deg360; // no type conversion  
   Rad2Pi radiRadenkovics; // no default constructor  @NOTE: auto-generated members are also tried!!
   Rad2Pi radi_1 = 4L;     // no constructor taking 1 element
#endif
   
   deg180 = (deg360 > 179 ? ((deg360 % 360) - 360) : (deg360 % 360));   
   SHOW(deg180, "after conversion NOTE: uses implicit conversion to POD");
   //...
}


#endif // LIMITED_INT_H_INCLUDED

