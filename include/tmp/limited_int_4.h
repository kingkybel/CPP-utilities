#ifndef LIMITED_INT_H_INCLUDED
#define LIMITED_INT_H_INCLUDED

template <  typename T_,
            T_ min_ = std::numeric_limits<T_>::min() + 1,
            T_ max_ = std::numeric_limits<T_>::max()
         >
struct limited_int
{
private:
   T_ val_ = min_;

public:

   limited_int(T_ val = min_)
   {
      //@NOTE: guard against abuse
      static_assert(true == std::is_integral<T_>::value,
                    "limited_int<> needs integral type as template parameter");

      static_assert(min_ < max_,
                    "limited_int<> min needs to be smaller than max");
      static_assert((min_ != std::numeric_limits<T_>::min()) || (max_ != std::numeric_limits<T_>::max()),
                    "either min or max must be not equal numeric_limits min() and max()");
      // @NOTE
      
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

   template<typename T2_, T2_ min2_, T2_ max2_>
   limited_int(limited_int<T2_, min2_, max2_> const & rhs)
   {
      long double distLhs = (max() - min());
      long double distRhs = (rhs.max() - rhs.min());
      long double valRhsTo0 = ((long double)rhs.val() -rhs.min());
      long double scaleFactor = distLhs/distRhs;
      long double valLhsTo0 = valRhsTo0 * scaleFactor;
      val_ = T_(valLhsTo0 + (long double)min());
   }

   static constexpr limited_int min()
   {
      return min_;
   }

   static constexpr limited_int max()
   {
      return max_;
   }

   T_ val() const
   {
      return val_;
   }

   operator T_() const
   {
      return val();
   }

   friend std::ostream & operator<<(std::ostream & os,
                                    limited_int<T_, min_, max_> const & i)
   {
      os << i.val()
            << " "
            << "["
            << (T_) i.min()
            << ","
            << (T_) i.max()
            << "]";
      return os;
   }

};

typedef limited_int<int64_t, -179, 180> Deg180;
typedef limited_int<int64_t, 0, 359> Deg360;
typedef limited_int<int64_t, 0, MICRO_RAD_2PI> Rad2Pi;

typedef limited_int<int64_t, -1'000'000, 1'000'000> MilliM;
typedef limited_int<int64_t, -1'000'000'000, 1'000'000'000> MicroM;
typedef limited_int<int64_t, 0, 2'000'000> MilliM2Million;

void execute()
{
   SHOW0("========= ADDED STATIC ASSERTIONS TO GUARD AGAINST ABUSE ================");
   Deg360 deg360 = 270;         // ok
   Deg180 deg180 = -90;         // ok
   Rad2Pi rad2Pi = 1'234'567;   // ok
   
   deg360 = 359;
   
   deg180 = deg360; // > 180!
   SHOW("deg180 = deg360","");
   SHOW(deg180, "valid after assignment of 360 value to 180 value, but NOT intuitive in this context");

   
   MilliM milliM = -567'000;
   MicroM microM = milliM;
   SHOW(milliM, "");
   SHOW(microM,"natural scaling conversion");
   MilliM2Million mm2Mio = milliM;
   SHOW(mm2Mio,"natural scaling conversion");
   
   MilliM milliMStrange = 1'500'000;
   SHOW(milliMStrange,"Strange behavior for linear (milliMStrange = 1'500'000)");
   
#ifdef SHOW_COMPILE_ERROR
   limited_int<int, 5, -3> strange1;
   SHOW(strange1,"strange way to define a limited int min > max");
   limited_int<double, 1.6, 3.1415> strange2;
   SHOW(strange2,"strange way to define a limited int");
   limited_int<bool, false, true> strange3;
   SHOW(strange3,"strange way to define a limited int -- whole range");

   limited_int<bool, true, true> strange4;
   SHOW(strange4,"strange way to define a limited int -- min == max");
#endif
}


#endif // LIMITED_INT_H_INCLUDED

