#ifndef LIMITED_INT_H_INCLUDED
#define LIMITED_INT_H_INCLUDED

template <  typename T_ = int32_t,
            T_ min_ = std::numeric_limits<T_>::min() + 1,   // @NOTE: defaulted constexpr value
            T_ max_ = std::numeric_limits<T_>::max()        // @NOTE: defaulted constexpr value
         >
struct limited_int
{
private:
   T_ val_ = min_;

public:

   limited_int(T_ val = min_)                 // @NOTE: NON-explicit default constructor
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

   static constexpr limited_int min()          // @NOTE: constexpr value
   {
      return min_;
   }

   static constexpr limited_int max()           // @NOTE: constexpr value
   {
      return max_;
   }

   T_ val() const
   {
      return val_;
   }

   operator T_() const                          // @NOTE: NOT explicit!
   {
      return val();
   }


   // @NOTE: global operator defined as friend within the template body
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

typedef limited_int<long, -179, 180> Deg180;          // @NOTE: type with min/max
typedef limited_int<long, 0, 359> Deg360;             // @NOTE: type with min/max
typedef limited_int<long, 0, MICRO_RAD_2PI> Rad2Pi;   // @NOTE: type with min/max

void execute()
{
   SHOW0("======= BETTER TEMPLATE: TYPE + MIN/MAX + STREAM OPERATOR =========");
   Deg360 deg360 = 7654321;         // ok
   SHOW(deg360, "valid deg360 = 7654321");
   
   Deg180 deg180 = -9876543;         // ok
   SHOW(deg180, "valid deg180 = -9876543");
   
   Rad2Pi rad2Pi = numeric_limits<long>::max();   // ok
   SHOW(rad2Pi, "valid rad2Pi = numeric_limits<long>::max()");

   SHOW0("--------- SIZEOF ---------");
   SHOW(sizeof(Deg360),"");
   SHOW(sizeof(long),"");
   SHOW0("--------------------------");
   
   deg360 = 359;
#ifdef SHOW_COMPILE_ERROR
   deg180 = deg360; // > 180!
#endif
   
   deg180 = (deg360 > 179 ? ((deg360 % 360) - 360) : (deg360 % 360));   
   SHOW(deg180, "after conversion NOTE: uses implicit conversion to POD");
   //...
}


#endif // LIMITED_INT_H_INCLUDED

